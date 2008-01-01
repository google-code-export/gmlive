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
		void mute(bool);
		virtual void stop();

		bool running() const;
		bool pausing() const { return is_pause; }
		void set_out_slot(const sigc::slot<bool, Glib::IOCondition>& slot)
		{ out_slot = slot; }

		ssize_t get_mplayer_log(char* buf, size_t count) 
		{ return read(child_tem, buf, count); }

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop()
		{ return signal_stop_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start()
		{ return signal_start_; }	
	protected:
		int my_system(char* const argv[]);
		int 		cache;
	private:
		void wait_mplayer_exit(GPid, int);
		virtual void on_mplayer_exit() {};
		void set_s_pipe();
		void set_m_pipe();
		void create_pipe();
		void close_pipe();

		sigc::slot<bool, Glib::IOCondition> 	out_slot;
		sigc::connection 		ptm_conn;
		sigc::connection 		wait_conn;
		type_signal_stop  		signal_stop_;
		type_signal_start 		signal_start_;
		int		child_tem; 	// 主进程和子进程的连接管道的主进程则
		int 		child_tem2; 	// 子进程端的标准输入，输出
		int		child_pid;	/* mplayer's pid (internal)*/
		bool		is_running;	
		bool		is_pause;	
};

#endif
