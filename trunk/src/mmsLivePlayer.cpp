/*
 * =====================================================================================
 *
 *       Filename:  mmsLivePlayer.cpp
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

#include "mmsLivePlayer.h"
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
	gmp.set_cache(8192);
	gmp.play(filename);
}

void MmsLivePlayer::stop()
{
	//gmp.stop();
}

