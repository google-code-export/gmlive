/*
 * =====================================================================================
 * 
 *       Filename:  ns_livePlayer.h
 * 
 *    Description:  nslive播放管理器
 * 
 *        Version:  1.0
 *        Created:  2007年12月03日 16时55分13秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _NS_LIVE_PLAYER_H_
#define _NS_LIVE_PLAYER_H_

#include "livePlayer.h"
#include "gmlive.h"

class NsLivePlayer : public LivePlayer {
	public:
		NsLivePlayer(const std::string& id_);
		~NsLivePlayer();
		void start(GMplayer& gmp);
		void stop();
		const std::string& get_stream() { return id; }
	private:
		bool on_gmp_startup_time();
		sigc::connection		gmp_startup_time_conn;
		std::string 		id;
		GMplayer* gmplayer;
		int 			ns_pid;
		bool			is_running;
};

#endif // _NS_LIVE_PLAYER_H_

