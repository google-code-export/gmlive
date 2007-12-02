#include "gmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
#include <sstream>
#include <sys/wait.h>
#include "MainWindow.h"


GMplayer::GMplayer(MainWindow* parent_):
	parent(parent_),
	ready(true),
	childpid(-1),
	xid(-1),
	timer(0)
{

	ptm_conn = Glib::signal_io().connect(
			sigc::mem_fun(*this, &GMplayer::on_callback),
			pst_ctrl.get_ptm(), Glib::IO_IN);

}


GMplayer::~GMplayer()
{
	stop();
}

bool GMplayer::on_callback(const Glib::IOCondition& condition)
{
	char buf[256];
	while (int len = read(pst_ctrl.get_ptm(), buf, 255)) {
		if (len < 256) {
			buf[len] = 0;
			printf("%s", buf);
			//parent->showMsg(buf);
			break;
		}
		printf("-%s", buf);
	}
	return true;
}

void GMplayer::wait_mplayer_exit(GPid pid, int)
{
	if (childpid != -1) {
		ptm_conn.disconnect();

		waitpid(childpid, NULL, 0);
		childpid = -1;

		pst_ctrl.close_ptm();
		ready = true;
	}
}

int GMplayer::my_system(const std::string& cmd)
{
	extern char **environ;

	int pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {

		pst_ctrl.setup_slave();

		const char* argv[4] = {"sh", "-c", cmd.c_str()};
		execve("/bin/sh", (char**)argv, environ);

		exit(127);
	} 

	childpid = pid;
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &GMplayer::wait_mplayer_exit), pid);

	pst_ctrl.wait_slave();
	std::cout << pid << std::endl;

}

bool GMplayer::is_runing()
{
	return ready && (childpid > 0);
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
	if (file.empty())
		return;

	if (is_runing()) {
		stop();
	}

	Glib::RefPtr<Gdk::Window> gwin = this->get_window();
	if(gwin)
		xid=GDK_WINDOW_XID(gwin->gobj());

	std::stringstream sst;
	sst << "mplayer " << 
		"-wid " << xid << " " << file ;
	ready = false;
	my_system(sst.str());
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
void GMplayer::stop()
{
	int fd = pst_ctrl.get_ptm();
	char ch = 'q';
	write(fd, &ch, 1);
}
void GMplayer::nslive_play()
{
	std::string filename = "http://127.0.0.1:9000" ;
	start(filename);
}

