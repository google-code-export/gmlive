/*
 * =====================================================================================
 *
 *       Filename:  nsLivePlayer.cpp
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
#include "nsLivePlayer.h"
#include <gmplayer.h>
#include "ec_throw.h"

NsLivePlayer::NsLivePlayer(const std::string& id_) : 
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

		const char* argv[4];
       		argv[0] = "nslive";
		argv[1] = "-p";
		argv[2] = id.c_str();
		argv[3] = NULL;

		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));

		execvp("nslive", (char* const *)argv);
		perror("nslive execvp:");
		exit(127);
	} 

	ns_pid = pid;
	printf("ns_pid = %d\n",ns_pid);
	gmp.set_cache(64);
	gmp.play(NSLIVESTREAM);
}

void NsLivePlayer::stop()
{
	if (ns_pid > 0) {
		for (;;) {
			kill(-ns_pid, SIGKILL);
			int ret = (waitpid( -ns_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}
		ns_pid = -1;
	}
}

