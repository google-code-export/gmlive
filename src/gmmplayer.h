#ifndef GTKMMPLAYER_H_
#define GTKMMPLAYER_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>


class GMMplayer : public Gtk::VBox
{
	public:
		GMMplayer();
		~GMMplayer();
		void show();
		void start(const std::string&);
		void start();
		void stop();
	private:
		int my_system(const std::string&);
		void change_size(Gtk::Allocation& allocation);
		bool is_runing();

		Gtk::Socket 	mySocket;       /* the socket (internal)*/
		std::string	file;		/* filename (internal)*/
		guint32		xid;		/* X window handle (internal)*/
		int		width;		/* widget's width*/
		int		height;		/* widget's height*/
		bool		ready;		/* is the player ready (internal)*/
		int		childpid;	/* mplayer's pid (internal)*/
		guint		timer;		/* timer (internal) */
};

#endif
