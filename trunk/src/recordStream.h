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
		void set_live_player(LivePlayer* lp, const std::string& name);
		LivePlayer* get_live_player() { return live_player; }
	private:
		void on_mplayer_exit();
		bool on_mplayer_out(const Glib::IOCondition& condition);
		void initialize();
		void set_out_file(const std::string& filename);

		std::string record_channel_name;
		std::string filename;
		std::string outfilename;
		LivePlayer* live_player;
};


#endif // _RECORD_STREAM_H_

