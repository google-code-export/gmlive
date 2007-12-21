#ifndef GTKMPLAYER_H_
#define GTKMPLAYER_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>
//#include "pst_ctrl.h"

class GMplayer : public Gtk::Socket
{
	public:
		GMplayer(const sigc::slot<bool, Glib::IOCondition>& slot);
		~GMplayer();
		void send_ctrl_command(const char* c);
		void set_cache(int cache_) { cache = cache_; }
		void play(const std::string&);
		void play();
		void record(const std::string&, const std::string&);
		void pause(); 
		void stop();
		void full_screen();
		void set_embed(bool embed_);
		bool is_recorded() const { return is_record; }
		ssize_t get_mplayer_log(char* buf, size_t count) 
		{ return read(stdout_pipe[0], buf, count); }

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop_play()
		{ return signal_stop_play_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start_play()
		{ return signal_start_play_; }	
	private:
		void initialize_play();
		void initialize_record(const std::string& outfilename);	
		bool is_runing();
		void set_s_pipe();
		void set_m_pipe();
		void create_pipe();
		void close_pipe();
		void wait_mplayer_exit(GPid, int);
		int my_system(char* const argv[]);
		bool on_startup_play_time();


		sigc::slot<bool, Glib::IOCondition> 	child_call;
		sigc::connection 			ptm_conn;
		sigc::connection 			wait_conn;
		type_signal_stop  			signal_stop_play_;
		type_signal_start 			signal_start_play_;
		int stdout_pipe[2];
		int stdin_pipe[2];
		std::string	file;		/* filename (internal)*/
		std::string	outfile;	/* filename (internal)*/
		int		width;		/* widget's width*/
		int		height;		/* widget's height*/
		int		childpid;	/* mplayer's pid (internal)*/
		int 		cache;
		guint32		xid;		/* X window handle (internal)*/
		bool		ready;		/* is the player ready (internal)*/
		bool		mode;
		bool		is_pause;
		bool		is_embed;
		bool		is_record;
};

#endif
