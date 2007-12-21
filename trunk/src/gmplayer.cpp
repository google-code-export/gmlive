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
#include <list>

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

GMplayer::GMplayer(const sigc::slot<bool, Glib::IOCondition>& slot):
	ready(true),
	childpid(-1),
	xid(-1),
	child_call(slot)
	,mode(1)
	,cache(64)
	,is_pause(false)
	,is_embed(true)
	,is_record(false)
{
	stdin_pipe[0] = -1;
	stdin_pipe[1] = -1;
	stdout_pipe[0] = -1;
	stdout_pipe[1] = -1;
}


GMplayer::~GMplayer()
{
	stop();
}

void GMplayer::wait_mplayer_exit(GPid pid, int)
{
	if (childpid != -1) {
		ptm_conn.disconnect();

		waitpid(childpid, NULL, 0);
		childpid = -1;

		ready = true;
		signal_stop_play_.emit();
	}

}

int GMplayer::my_system(char* const argv[])
{
	extern char **environ;
	create_pipe();
	ptm_conn = Glib::signal_io().connect(
			child_call,
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
	childpid = pid;

	set_m_pipe();


	wait_conn = Glib::signal_child_watch().connect(
			sigc::mem_fun(*this, &GMplayer::wait_mplayer_exit), pid, 0);

	return pid;
}

bool GMplayer::is_runing()
{
	return (!ready) && (childpid > 0);
}

void GMplayer::initialize_play()
{
	signal_start_play_.emit();

	Glib::RefPtr<Gdk::Window> gwin = this->get_window();

	char cache_buf[32];
	snprintf(cache_buf, 32, "%d", cache);

	std::string& paramter = GMConf["mplayer_paramter"];
	std::list<std::string> pars;
	if (!paramter.empty()) {
		size_t pos1 = 0;
		size_t pos2 = 0;
		for(;;) {
			pos2 = paramter.find_first_of(" \t", pos1);
			pars.push_back(paramter.substr(pos1, pos2 - pos1));
			if (pos2 == std::string::npos)
				break;
			pos1 = pos2 + 1;
		}
	}

	int argv_len = 10 + pars.size();
	const char* argv[argv_len];
	argv[0] = "mplayer";
	argv[1] = "-slave";
	argv[2] = "-idle";
	argv[3] = "-quiet";
	argv[4] = "-cache";
	argv[5] = cache_buf;
	std::list<std::string>::iterator iter = pars.begin();
	int i = 6;
	for (; i < argv_len && iter != pars.end(); i++, ++iter) {
		argv[i] = (*iter).c_str();
	}
	if (is_embed) {
		char wid_buf[32];
		snprintf(wid_buf, 32, "%d", this->get_id());
		printf("%s\n", wid_buf);
		argv[i++] = "-wid";
		argv[i++] = wid_buf;
	}
	argv[i] = NULL;

	ready = false;
	my_system((char* const *) argv);
}

void GMplayer::play()
{
	is_record = false;
	if (is_pause)
		return pause();
	if (is_runing())
		stop();
	initialize_play();
	char cb[256];
	int len = snprintf(cb, 256, "loadfile %s\n", file.c_str());
	EC_THROW(-1 == write(stdin_pipe[1], cb, len));

}

void GMplayer::play(const std::string& filename)
{
	if (filename != file)
		file = filename;
	play();
}

void GMplayer::initialize_record(const std::string& outfilename)
{
	signal_start_play_.emit();
	char cache_buf[32];
	snprintf(cache_buf, 32, "%d", cache);
	const char* argv[10];
	argv[0] = "mplayer";
	argv[1] = "-slave";
	argv[2] = "-idle";
	argv[3] = "-cache";
	argv[4] = cache_buf;
	argv[5] = "-dumpstream";
	argv[6] = "-dumpfile";
	argv[7] = outfilename.c_str();
	argv[8] = "-quiet";
	argv[9] = NULL;

	ready = false;
	my_system((char* const *) argv);
}

void GMplayer::record(const std::string& filename,
		const std::string& outfilename)
{
	is_record = true;
	file = filename;
	outfile = outfilename;
	if (is_runing())
		stop();
	initialize_record(outfilename);
	char cb[256];
	int len = snprintf(cb, 256, "loadfile %s\n", file.c_str());
	EC_THROW(-1 == write(stdin_pipe[1], cb, len));
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GMplayer::on_startup_play_time), 3);
}

bool GMplayer::on_startup_play_time()
{
	std::cout << "startup mplayer" << std::endl;
	return false;
}

void GMplayer::full_screen()
{
}

void GMplayer::send_ctrl_command(const char* c)
{
	if (is_runing()) {
		assert(c != NULL);
		EC_THROW(-1 == write(stdin_pipe[1], c, strlen (c)));
	}
}

void GMplayer::stop()
{
	if (childpid != -1) {
		ptm_conn.disconnect();
		wait_conn.disconnect();

		for (;;) {
			kill(-childpid, SIGKILL);
			int ret = (waitpid( -childpid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}

		childpid = -1;	
		signal_stop_play_.emit();
	}
	ready = true;
}


void GMplayer::pause()
{
	is_pause = !is_pause;
	send_ctrl_command("pause\n");
}

void GMplayer::set_embed(bool embed_)
{
	if (is_embed == embed_)
		return;
	is_embed = embed_;
	if (is_runing()) 
		play();
}

