#ifndef GTKMPLAYER_H_
#define GTKMPLAYER_H_

#include <stdlib.h>
#include <iostream>
//#include "pst_ctrl.h"
#include "gmlive.h"

class GMplayer {
	public:
		GMplayer();
		~GMplayer();
		void send_ctrl_command(const char* c);
		void set_cache(int cache_) { cache = cache_; }
		virtual void start(const std::string&) = 0;
		virtual void start() = 0;
		void pause(); 
		virtual void stop();

		void set_out_slot(const sigc::slot<bool, Glib::IOCondition>& slot)
		{ out_slot = slot; }

		ssize_t get_mplayer_log(char* buf, size_t count) 
		{ return read(stdout_pipe[0], buf, count); }

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop()
		{ return signal_stop_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start()
		{ return signal_start_; }	
	protected:
		bool running();
		void set_s_pipe();
		void set_m_pipe();
		void create_pipe();
		void close_pipe();
		void wait_mplayer_exit(GPid, int);
		int my_system(char* const argv[]);

		sigc::slot<bool, Glib::IOCondition> 	out_slot;
		sigc::connection 		ptm_conn;
		sigc::connection 		wait_conn;
		type_signal_stop  		signal_stop_;
		type_signal_start 		signal_start_;
		int 				stdout_pipe[2];
		int 				stdin_pipe[2];
		int		child_pid;	/* mplayer's pid (internal)*/
		int 		cache;
		bool		is_running;	
		bool		is_pause;	
	private:
		virtual void on_mplayer_exit() {};
};

#endif
