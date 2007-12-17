/*
 * =====================================================================================
 * 
 *       Filename:  sopcast_livePlayer.h
 * 
 *    Description:  sopcast播放器类
 * 
 *        Version:  1.0
 *        Created:  2007年12月04日 20时12分55秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  SOPCAST_LIVE_PLAYER_FILE_HEADER_INC
#define  SOPCAST_LIVE_PLAYER_FILE_HEADER_INC
#include "livePlayer.h"

class GMplayer;
class SopcastLivePlayer : public LivePlayer {
	public:
		SopcastLivePlayer(const std::string& stream_);
		~SopcastLivePlayer();
		void play(GMplayer&);
		void stop();
		const std::string& get_stream() { return stream; }

	protected:
		bool on_sop_sock(const Glib::IOCondition& condition);
		bool on_sop_time_status();
		
	private:
		sigc::connection sop_time_conn;
		sigc::connection sop_sock_conn;
		GMplayer* gmplayer;
		std::string stream;
		int sop_pid;
		int sop_sock;
		bool player;
};


#endif   /* ----- #ifndef SOPCAST_LIVE_PLAYER_FILE_HEADER_INC  ----- */

