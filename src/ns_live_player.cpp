/*
 * =====================================================================================
 *
 *       Filename:  ns_live_player.cpp
 *
 *    Description:  nslive播放管理器
 *
 *        Version:  1.0
 *        Created:  2007年12月03日 17时31分20秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "gmlive.h"
#include "ns_live_player.h"


NsLivePlayer::NsLivePlayer(GMplayer& gmp, int id_) : 
	LivePlayer(gmp),
	id(id_),
	ns_pid(-1)
{
}

NsLivePlayer::~NsLivePlayer()
{
	stop();
	printf("nslive exit\n");
}

void NsLivePlayer::play()
{
	extern char **environ;
	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		char buf[256];
		sprintf(buf, "nslive -p %d", id);
		const char* argv[4] = {"sh", "-c", buf};
		execve("/bin/sh", (char**)argv, environ);
		exit(127);
	} 

	ns_pid = pid;
	printf("%d",ns_pid);
	//gmp.start("http://127.0.0.1:9000");
	gmp.start(NSLIVESTREAM);
}

void NsLivePlayer::stop()
{
	gmp.stop();
	if (ns_pid > 0) {
		kill(ns_pid, SIGKILL);
		waitpid(ns_pid, NULL, 0);
		ns_pid = -1;
	}

}

