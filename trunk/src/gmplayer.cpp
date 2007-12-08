#include "gmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
//#include <sstream>
#include <sys/wait.h>
#include "ec_throw.h"
void GMplayer::set_s_pipe()
{
	close(stdin_pipe[1]);
	close(stdout_pipe[0]);
	EC_THROW( -1 == dup2(stdin_pipe[0], STDIN_FILENO));
	EC_THROW( -1 == dup2(stdout_pipe[1], STDOUT_FILENO));
	EC_THROW( -1 == dup2(stdout_pipe[1], STDERR_FILENO));
}

GMplayer::GMplayer(const sigc::slot<bool, Glib::IOCondition>& slot):
	ready(true),
	childpid(-1),
	xid(-1),
	timer(0),
	replay(false), 
	child_call(slot)
	//pst_ctrl(NULL)
{
	EC_THROW(-1 == pipe(stdin_pipe));
	EC_THROW(-1 == pipe(stdout_pipe));
}


GMplayer::~GMplayer()
{
	stop();
}

	
//
//bool GMplayer::on_callback(const Glib::IOCondition& condition)
//{
//	char buf[256];
//	while (int len = read(pst_ctrl->get_ptm(), buf, 255)) {
//		if (len < 256) {
//			buf[len] = 0;
//			printf("%s", buf);
//			//parent->showMsg(buf);
//			break;
//		}
//		printf("-%s", buf);
//	}
//	return true;
//}

void GMplayer::wait_mplayer_exit(GPid pid, int)
{
	if (childpid != -1) {
		ptm_conn.disconnect();

		waitpid(childpid, NULL, 0);
		childpid = -1;

		//delete pst_ctrl;
		//pst_ctrl = NULL;

		ready = true;
	}
	
	signal_stop_play_.emit();

	if (replay) {
		start();
	}
}

int GMplayer::my_system(char* const argv[])
{
	extern char **environ;

	//pst_ctrl = new PstCtrl;

	ptm_conn = Glib::signal_io().connect(
			child_call,
			stdout_pipe[0], Glib::IO_IN);


	int pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {


		set_s_pipe();
		//const char* argv[2] = {"mplayer", cmd.c_str()};
		//execle("/usr/bin/mplayer", cmd.c_str(), environ);
		execvp(argv[0], argv);

		perror("mplayer execvp:");
		exit(127);
	} 
	childpid = pid;
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &GMplayer::wait_mplayer_exit), pid);


}

bool GMplayer::is_runing()
{
	return (!ready) && (childpid > 0);
}
/*
void GMplayer::stop()
{
	if (childpid > 0) {
		kill(childpid, SIGKILL);
		kill(childpid+1, SIGKILL);
		childpid = -1;
	}
	ready = true;
}
*/

void GMplayer::start()
{
	if (is_runing()) {
		stop();
		replay = true; // 正在播放，要等到当前的mplayer停止再启动新的
		return;
	}

	replay = false;

	if (file.empty())
		return;

	signal_start_play_.emit();

	Glib::RefPtr<Gdk::Window> gwin = this->get_window();
	if(gwin)
		xid=GDK_WINDOW_XID(gwin->gobj());

	//char wid_buf[32];
	//sprintf(wid_buf, "%d", xid);
	char command_buf[64];
	sprintf(command_buf, "mplayer -wid %d %s", xid, file.c_str());

	const char* argv[5];
       	argv[0] = "sh";
	argv[1] = "-c";
	argv[2] = command_buf;
	argv[3] = NULL;
	//argv[0] = "mplayer";
	//argv[1] = "-wid";
	//argv[2] = wid_buf;
	//argv[3] = file.c_str();
	//argv[4] = NULL;

	ready = false;
	my_system((char* const *) argv);
}

void GMplayer::start(const std::string& filename)
{
	if (filename != file)
		file = filename;
	start();
}

void GMplayer::full_screen()
{
}

void GMplayer::send_ctrl_word(char c)
{
	//if (pst_ctrl) {
		//EC_THROW(-1 == write(pst_ctrl->get_ptm(), &c, sizeof (char)));
		EC_THROW(-1 == write(stdin_pipe[1], &c, sizeof (char)));
	//}
}


