#include "gmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "ec_throw.h"
#include <cassert>
#include "gmlive.h"
#include <string>

void  set_signals()
{
	sigset_t set;
	struct sigaction act;

	EC_THROW( -1 == sigfillset(&set));
	EC_THROW( -1 == sigprocmask(SIG_SETMASK, &set, NULL));

	memset(&act, 0, sizeof(act));

	EC_THROW( sigfillset(&act.sa_mask));
	act.sa_handler = SIG_IGN;
	EC_THROW( sigaction(SIGHUP, &act, NULL));
	EC_THROW( sigaction(SIGINT, &act, NULL));
	EC_THROW( sigaction(SIGQUIT, &act, NULL));
	EC_THROW( sigaction(SIGPIPE, &act, NULL));

	EC_THROW( sigaction(SIGTERM, &act, NULL));
	EC_THROW( sigaction(SIGBUS, &act, NULL));
	EC_THROW( sigaction(SIGFPE, &act, NULL));
	EC_THROW( sigaction(SIGILL, &act, NULL));
	EC_THROW( sigaction(SIGSEGV, &act, NULL));
	EC_THROW( sigaction(SIGSYS, &act, NULL));
	EC_THROW( sigaction(SIGXCPU, &act, NULL));
	EC_THROW( sigaction(SIGXFSZ, &act, NULL));
	EC_THROW( sigaction(SIGHUP, &act, NULL));

	EC_THROW( sigemptyset(&set));
	EC_THROW( sigprocmask(SIG_SETMASK, &set, NULL));
	
}

void GMplayer::create_pipe()
{
	close_pipe();

	EC_THROW(-1 == pipe(stdin_pipe));
	EC_THROW(-1 == pipe(stdout_pipe));
}

void GMplayer::close_pipe()
{
	if (stdin_pipe[0] != -1) {
		close(stdin_pipe[0]);
		stdin_pipe[0] = -1;
	}
	if (stdin_pipe[1] != -1) {
		close(stdin_pipe[1]);
		stdin_pipe[1] = -1;
	}
	if (stdout_pipe[0] != -1) {
		close(stdout_pipe[0]);
		stdout_pipe[0] = -1;
	}
	if (stdout_pipe[1] != -1) {
		close(stdout_pipe[1]);
		stdout_pipe[1] = -1;
	}
}

void GMplayer::set_s_pipe()
{
	close(stdin_pipe[1]);
	close(stdout_pipe[0]);
	EC_THROW( -1 == dup2(stdin_pipe[0], STDIN_FILENO));
	EC_THROW( -1 == dup2(stdout_pipe[1], STDOUT_FILENO));
	//EC_THROW( -1 == dup2(stdout_pipe[1], STDERR_FILENO));
}

void GMplayer::set_m_pipe()
{
	close(stdin_pipe[0]);
	stdin_pipe[0] = -1;
	close(stdout_pipe[1]);
	stdout_pipe[1] = -1;

	fd_set fd_set_write;
	FD_ZERO(&fd_set_write);
	FD_SET(stdin_pipe[1], &fd_set_write);

	EC_THROW( -1 == select (stdin_pipe[1] + 1, NULL, &fd_set_write, NULL, NULL));

}


////////////////////////////////////////////////////////////////

GMplayer::GMplayer()
	:is_running(false)
	,is_pause(false)
	,child_pid(-1)
	,cache(64)
{
	stdin_pipe[0] = -1;
	stdin_pipe[1] = -1;
	stdout_pipe[0] = -1;
	stdout_pipe[1] = -1;
}


GMplayer::~GMplayer()
{
}

void GMplayer::wait_mplayer_exit(GPid pid, int)
{
	if (child_pid != -1) {
		ptm_conn.disconnect();

		waitpid(child_pid, NULL, 0);
		child_pid = -1;

		is_running = false;
		on_mplayer_exit();
	}

}

int GMplayer::my_system(char* const argv[])
{
	extern char **environ;
	create_pipe();
	assert(!out_slot.empty());
	ptm_conn = Glib::signal_io().connect(
			out_slot,
			stdout_pipe[0], Glib::IO_IN);


	pid_t pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {

		set_signals();
		set_s_pipe();
		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		//sleep(4);
		execvp(argv[0], argv);

		perror("mplayer execvp:");
	} 
	child_pid = pid;

	set_m_pipe();


	wait_conn = Glib::signal_child_watch().connect(
			sigc::mem_fun(*this, &GMplayer::wait_mplayer_exit), pid, 0);

	is_running = true;
	return pid;
}

bool GMplayer::running()
{
	return (is_running) && (child_pid > 0);
}


void GMplayer::send_ctrl_command(const char* c)
{
	if (running()) {
		assert(c != NULL);
		EC_THROW(-1 == write(stdin_pipe[1], c, strlen (c)));
	}
}

void GMplayer::stop()
{
	if (child_pid != -1) {
		ptm_conn.disconnect();
		wait_conn.disconnect();

		for (;;) {
			kill(-child_pid, SIGKILL);
			int ret = (waitpid( -child_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}

		child_pid = -1;	
	}
	is_running = false;
}


void GMplayer::pause()
{
	is_pause = !is_pause;
	send_ctrl_command("pause\n");
}


