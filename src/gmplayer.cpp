#include "gmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
#include <sstream>


GMplayer::GMplayer():
	ready(true),
	childpid(-1),
	xid(-1),
	timer(0)
{
}


GMplayer::~GMplayer()
{
	stop();
}



int GMplayer::my_system(const std::string& cmd)
{
	extern char **environ;

	int pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {

		pst_ctrl.setup_slave();
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		//close(STDIN_FILENO);

		const char* argv[4] = {"sh", "-c", cmd.c_str()};
		execve("/bin/sh", (char**)argv, environ);

		exit(127);
	} 

	pst_ctrl.wait_slave();
	std::cout << pid << std::endl;
	childpid = pid;

}

bool GMplayer::is_runing()
{
	return ready && (childpid > 0);
}

void GMplayer::stop()
{
	if (childpid > 0) {
		kill(childpid, SIGKILL);
		kill(childpid+1, SIGKILL);
		childpid = -1;
	}
	ready = true;
}

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

