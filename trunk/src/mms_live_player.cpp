/*
 * =====================================================================================
 *
 *       Filename:  mms_live_player.cpp
 *
 *    Description:  mms播放管理器 
 *
 *        Version:  1.0
 *        Created:  2007年12月03日 16时25分41秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#include "mms_live_player.h"

MmsLivePlayer::MmsLivePlayer(GMplayer& gmp, const std::string& fname) :
	LivePlayer(gmp),
	filename(fname)
{
}

void MmsLivePlayer::play()
{
	gmp.start(filename);
}

void MmsLivePlayer::stop()
{
	gmp.stop();
}

