#ifndef GTKMPLAYER_H_
#define GTKMPLAYER_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>
#include "pst_ctrl.h"

class MainWindow;

class GMplayer : public Gtk::Socket
{
	public:
		GMplayer(MainWindow* parent_);
		~GMplayer();
		void send_ctrl_word(char c);
		void start(const std::string&);
		void start();
		void stop() { send_ctrl_word('q'); }
		void full_screen();
		void nslive_play();
	private:
		MainWindow* parent;
		void wait_mplayer_exit(GPid, int);
		int my_system(const std::string&);
		void change_size(Gtk::Allocation& allocation);
		bool on_callback(const Glib::IOCondition& condition);
		bool is_runing();

		sigc::connection ptm_conn;
		PstCtrl* 	pst_ctrl;
		std::string	file;		/* filename (internal)*/
		int		width;		/* widget's width*/
		int		height;		/* widget's height*/
		int		childpid;	/* mplayer's pid (internal)*/
		guint32		xid;		/* X window handle (internal)*/
		guint		timer;		/* timer (internal) */
		bool		ready;		/* is the player ready (internal)*/
		bool		replay;		/* 重新播放状态 */
};

#endif
