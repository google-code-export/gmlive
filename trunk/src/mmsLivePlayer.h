/*
 * =====================================================================================
 * 
 *       Filename:  mms_livePlayer.h
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

#include "livePlayer.h"
#include <string>

class MmsLivePlayer : public LivePlayer {
	public:
		static MmsLivePlayer* create(const std::string&name);
		~MmsLivePlayer();
		void start(GMplayer& gmp);
		void stop();
		const std::string& get_stream() { return filename; }
	protected:
		MmsLivePlayer(const std::string& fname);
	private:
		static MmsLivePlayer* self;
};

#endif // _MMS_LIVE_PLAYER_H_

