/*
 * =====================================================================================
 * 
 *       Filename:  sopcast_live_player.h
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
#include "live_player.h"

class SopcastLivePlayer : public LivePlayer {
	public:
		SopcastLivePlayer(GMplayer& gmp, const std::string& stream_);
		~SopcastLivePlayer();
		void play();
		void stop();

	protected:
		bool on_sop_sock(const Glib::IOCondition& condition);
		bool on_sop_time_status();
		
	private:
		sigc::connection sop_time_conn;
		sigc::connection sop_sock_conn;
		std::string stream;
		int sop_pid;
		int sop_sock;
		bool player;
};


#endif   /* ----- #ifndef SOPCAST_LIVE_PLAYER_FILE_HEADER_INC  ----- */

