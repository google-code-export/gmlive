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
	private:
		int sop_pid;
		std::string stream;
};


#endif   /* ----- #ifndef SOPCAST_LIVE_PLAYER_FILE_HEADER_INC  ----- */

