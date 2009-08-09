/*
 * =====================================================================================
 *
 *       Filename:  pplivePlayer.h
 *
 *    Description:  pplive播放器类
 *
 *        Version:  1.0
 *        Created:  2009年08月06日 20时04分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#ifndef PPLIVE_PLAYER_H_
#define PPLIVE_PLAYER_H_
#include "livePlayer.h"

class GMplayer;
class PpLivePlayer : public LivePlayer {
	public:
		static PpLivePlayer* create(const std::string& stream_);
		~PpLivePlayer();
		void start(GMplayer&);
		void stop();

	protected:
		PpLivePlayer(const std::string& stream_);
		bool on_sop_sock(const Glib::IOCondition& condition);
		bool on_sop_time_status();
		
	private:
		sigc::connection sop_time_conn;
		sigc::connection sop_sock_conn;
		GMplayer* gmplayer;
		static PpLivePlayer* self;
		int pplive_pid;
		int sop_sock;
		bool is_running;
};


#endif // PPLIVE_PLAYER_H_

