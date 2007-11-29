#ifndef GTKMPLAYER_H_
#define GTKMPLAYER_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>

#include "pst_ctrl.h"


class GMplayer : public Gtk::Socket
{
	public:
		GMplayer();
		~GMplayer();
		void start(const std::string&);
		void start();
		void stop();
	private:
		int my_system(const std::string&);
		void change_size(Gtk::Allocation& allocation);
		bool on_callback(Glib::IOCondition condition);
		bool is_runing();

		PstCtrl 	pst_ctrl;
		std::string	file;		/* filename (internal)*/
		guint32		xid;		/* X window handle (internal)*/
		int		width;		/* widget's width*/
		int		height;		/* widget's height*/
		bool		ready;		/* is the player ready (internal)*/
		int		childpid;	/* mplayer's pid (internal)*/
		guint		timer;		/* timer (internal) */
};

#endif
