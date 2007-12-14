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
#include <gmplayer.h>

MmsLivePlayer::MmsLivePlayer(const std::string& fname) :
	filename(fname)
{
}

MmsLivePlayer::~MmsLivePlayer()
{
	//gmp.stop();
	printf("mms exit\n");
}
void MmsLivePlayer::play(GMplayer& gmp)
{
	gmp.play(filename);
}

void MmsLivePlayer::stop()
{
	//gmp.stop();
}

