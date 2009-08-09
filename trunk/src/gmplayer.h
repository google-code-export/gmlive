/*
 * =====================================================================================
 * 
 *       Filename:  gmplayer.h
 * 
 *    Description:  播放网络流 
 * 
 *        Version:  1.0
 *        Created:  2007年12月22日 20时40分35秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */
#ifndef GTKMPLAYER_H_
#define GTKMPLAYER_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>

class GMplayer : public Gtk::Socket {
	public:
		GMplayer();
		~GMplayer();
		void send_ctrl_command(const char* c);
		void pause(); 
		void mute(bool);
		void stop();
		void restart() { stop(); start();}
		void start() 
		{
			is_record ? start_record() : start_play();
		}
		void start(const std::string& filename)
		{
			is_record ? start_record(filename) : start_play(filename);
		}
		

		void set_cache(int var) { icache = var; }
		void set_record(bool record)
	   	{ 
			if (is_record != record)
				stop();
			is_record = record; 
		}
		void set_outfilename(const std::string& filename, const std::string& name = "");
		bool running() const { return is_running; }
		bool pausing() const { return is_pause; }
		bool recording() const { return is_recording; }
		void set_out_slot(const sigc::slot<bool, Glib::IOCondition>& slot)
		{ out_slot = slot; }

		ssize_t get_mplayer_log(char* buf, size_t count) 
		{ return read(player_child_tem, buf, count); }

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop()
		{ return signal_stop_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start()
		{ return signal_start_; }	
	private:
		void 	start_play(const std::string&);
		void 	start_record(const std::string&);
		void 	start_play();
		void 	start_record();
		int 	my_system(char* const argv[]);
		bool 	on_delay_reboot();
		bool 	on_delay_record_reboot();
		void 	on_start_record();
		void 	on_stop_record();
		bool 	on_update_progress();
		void 	on_preview();
		void 	wait_mplayer_exit(GPid, int);
		void	wait_record_exit(GPid, int);
		void 	set_s_pipe();
		void 	set_m_pipe();
		void 	create_pipe();
		void 	close_pipe();

		sigc::slot<bool, Glib::IOCondition> 	out_slot;
		sigc::connection		update_progress_conn;
		sigc::connection 		ptm_conn;
		sigc::connection 		wait_conn;
		sigc::connection 		wait_record_conn;
		type_signal_stop  		signal_stop_;
		type_signal_start 		signal_start_;

		std::string playfilename;		/* filename (internal)*/
		std::string recordfilename;
		std::string channelname;
		std::string outfilename;
		std::string outfilenamebase;
		std::string outfilenameext;
		int 		outfile_num;
		int 		outfile;
		Gtk::Window* 		record_window;
		Gtk::Label* 		record_name;
		Gtk::ProgressBar*	progress_bar;

		int 		icache;
		int			player_child_tem; 	// 主进程和子进程的连接管道的主进程则(播放)
		int 		player_child_tem2; 	// 子进程端的标准输入，输出 (播放)

		int			player_pid;	/* play mplayer's pid (internal)*/
		int			record_pid;	/* record mplayer's pid (internal)*/
		bool		is_running;	
		bool		is_recording;
		bool 		is_record;
		bool		is_pause;	
};

#endif
