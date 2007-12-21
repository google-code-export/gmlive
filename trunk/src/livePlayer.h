/*
 * =====================================================================================
 * 
 *       Filename:  live_player.h
 * 
 *    Description:  管理播放的父类
 * 
 *        Version:  1.0
 *        Created:  2007年12月03日 16时13分21秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _LIVE_PLAYER_H_
#define _LIVE_PLAYER_H_

#include <gtkmm.h>

class GMplayer;
class LivePlayer {
	public:
		LivePlayer():record(false) {}
		virtual ~LivePlayer() {}
		virtual void start(GMplayer&) = 0;
		virtual void stop() = 0;
		virtual const std::string& get_stream() = 0;
		void set_record(bool record_ = true) { record = record_; }
		void set_record_filename(
				const std::string& outfilename_ )
	       	{ outfilename = outfilename_; }

		typedef sigc::signal<void, int> type_signal_status;
		type_signal_status signal_status()
		{ return signal_status_; }
	protected:
		std::string				outfilename;
		type_signal_status  			signal_status_;
		bool 					record;
};
#endif // _LIVE_PLAYER_H_

