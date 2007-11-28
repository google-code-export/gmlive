#include "gmmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
#include <sstream>


GMMplayer::GMMplayer():
	ready(true),
	childpid(-1),
	xid(-1),
	timer(0)
{

	this->pack_start(mySocket);
	this->signal_size_allocate().connect(sigc::mem_fun(*this,
				&GMMplayer::change_size));

}

GMMplayer::~GMMplayer()
{
}

void GMMplayer::change_size(Gtk::Allocation& allocation)
{
	//	Gtk::Window::on_realize();
	int status;
	this->width = allocation.get_width();
	this->height = allocation.get_height();
	if(ready)
		this->start();
}


void GMMplayer::show()
{
	mySocket.show();
}

int GMMplayer::my_system(const std::string& cmd)
{
	extern char **environ;

	int pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {
		const char* argv[4] = {"sh", "-c", cmd.c_str()};
//		argv[0] = "sh";
//		argv[1] = "-c";
//		argv[2] = cmd.c_str();
//		argv[3] = 0;
		execve("/bin/sh", (char**)argv, environ);
		exit(127);
	}

}

bool GMMplayer::is_runing()
{
	return ready && (childpid > 0);
}

void GMMplayer::stop()
{
	if (childpid > 0) {
		kill(childpid, SIGKILL);
		kill(childpid+1, SIGKILL);
		childpid = -1;
	}
	ready = true;
}

void GMMplayer::start()
{
	if (file.empty())
		return;

	if (is_runing()) {
		stop();
	}

	Glib::RefPtr<Gdk::Window> gwin = mySocket.get_window();
	if(gwin)
		xid=GDK_WINDOW_XID(gwin->gobj());

	std::stringstream sst;
	sst << "mplayer " << "-vo " << "xv " <<
	//sst << "mplayer " << "-vo " << "xv " << "-vf " <<
		//"scale=" << width << ":-3 " << 
		"-wid " << xid << " " << file << " &";
	//std::string cmd("mplayer -vo xv -vop scale = %d:-3 -wid %d %s &",
	//		width, xid, file);
	ready = false;

	std::cout << sst.str() << std::endl;
	my_system(sst.str());
}

void GMMplayer::start(const std::string& filename)
{
	if (filename != file)
		file = filename;
	start();
}

