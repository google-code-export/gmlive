/*
 * =====================================================================================
 *
 *       Filename:  ppsPlayer.h
 *
 *    Description:  pps播放器类
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

#ifndef PPSTREAM_PLAYER_H_
#define PPSTREAM_PLAYER_H_
#include "livePlayer.h"

class GMplayer;
class PPSPlayer : public LivePlayer {
	public:
		static PPSPlayer* create(const std::string& stream_);
		~PPSPlayer();
		void start(GMplayer&);
		void stop();

	protected:
		PPSPlayer(const std::string& stream_);
		bool on_sop_sock(const Glib::IOCondition& condition);
		bool on_sop_time_status();
		
	private:
		sigc::connection sop_time_conn;
		sigc::connection sop_sock_conn;
		GMplayer* gmplayer;
		static PPSPlayer* self;
		int pps_pid;
		int sop_sock;
		FILE* sop_file;
		bool is_running;
};


#endif // PPSTREAM_PLAYER_H_

