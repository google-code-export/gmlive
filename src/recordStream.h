/*
 * =====================================================================================
 * 
 *       Filename:  recordStream.h
 * 
 *    Description:  录制网络流
 * 
 *        Version:  1.0
 *        Created:  2007年12月22日 19时54分11秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _RECORD_STREAM_H_
#define _RECORD_STREAM_H_
#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gmplayer.h>
#include "livePlayer.h"

class RecordStream :public GMplayer , public Gtk::Window{
	public:
		RecordStream();
		~RecordStream();
		void start(const std::string& filename);
		void start();
		void set_live_player(LivePlayer* lp, const std::string& name = "");
		LivePlayer* get_live_player() { return live_player; }
		void set_out_file(const std::string& filename);
		typedef sigc::signal<void, std::string> type_signal_preview;
		type_signal_preview signal_preview()
		{ return m_signal_preview; }
	//protected:
		type_signal_preview m_signal_preview;
	private:
		void on_preview();
		void on_stop();
		bool on_delete_event(GdkEventAny* event);
		void on_live_player_out(int percentage);
		bool on_timeout();
		void on_mplayer_exit();
		bool on_mplayer_out(const Glib::IOCondition& condition);
		void initialize();
	
		sigc::connection	timeout_conn;
		std::string record_channel_name;
		std::string filename;
		std::string outfilename;
		int  outfile;
		LivePlayer* live_player;
		Gtk::Label* record_name;
		Gtk::ProgressBar* progress_bar;
};


#endif // _RECORD_STREAM_H_

