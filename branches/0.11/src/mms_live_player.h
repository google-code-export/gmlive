/*
 * =====================================================================================
 * 
 *       Filename:  mms_live_player.h
 * 
 *    Description:  mms播放管理器
 * 
 *        Version:  1.0
 *        Created:  2007年12月03日 16时19分27秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _MMS_LIVE_PLAYER_H_
#define _MMS_LIVE_PLAYER_H_

#include "live_player.h"
#include <string>

class MmsLivePlayer : public LivePlayer {
	public:
		MmsLivePlayer(GMplayer& gmp, const std::string& fname);
		~MmsLivePlayer();
		void play();
		void stop();
	private:
		std::string filename;
};

#endif // _MMS_LIVE_PLAYER_H_

