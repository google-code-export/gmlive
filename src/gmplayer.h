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
		GMplayer(const sigc::slot<bool, Glib::IOCondition>& slot);
		~GMplayer();
		void send_ctrl_word(char c);
		void start(const std::string&);
		void start();
		void stop() { send_ctrl_word('q'); }
		void full_screen();
		ssize_t get_mplayer_log(char* buf, size_t count) 
		{ return read(pst_ctrl->get_ptm(), buf, count); }

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop_play()
		{ return signal_stop_play_; }
		
		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start_play()
		{ return signal_start_play_; }	
	private:
		void wait_mplayer_exit(GPid, int);
		int my_system(const std::string&);
		void change_size(Gtk::Allocation& allocation);
		bool is_runing();

		sigc::slot<bool, Glib::IOCondition> 	child_call;
		sigc::connection 			ptm_conn;
		type_signal_stop  			signal_stop_play_;
		type_signal_start 			signal_start_play_;
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
