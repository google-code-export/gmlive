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
#include <gmplayer.h>

NsLivePlayer::NsLivePlayer(int id_) : 
	id(id_),
	ns_pid(-1)
{
}

NsLivePlayer::~NsLivePlayer()
{
	stop();
	printf("nslive exit\n");
}

void NsLivePlayer::play(GMplayer& gmp)
{
	//extern char **environ;
	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);

		char id_buf[32];
		sprintf(id_buf, "%d", id);

		const char* argv[4];
       		argv[0] = "nslive";
		argv[1] = "-p";
		argv[2] = id_buf;
		argv[3] = NULL;

		execvp("nslive", (char* const *)argv);
		perror("nslive execvp:");
		exit(127);
	} 

	ns_pid = pid;
	printf("%d\n",ns_pid);
	gmp.play(NSLIVESTREAM);
}

void NsLivePlayer::stop()
{
	//gmp.stop();
	if (ns_pid > 0) {
		kill(ns_pid, SIGKILL);
	//	kill(ns_pid+1, SIGKILL);
		waitpid(ns_pid, NULL, 0);
		ns_pid = -1;
	}

}

