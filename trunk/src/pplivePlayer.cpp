/*
 * =====================================================================================
 *
 *       Filename:  pplivePlayer.cpp
 *
 *    Description:  pplive播放器类
 *
 *        Version:  1.0
 *        Created:  2009年08月06日 20时07分01秒
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
#include <sys/socket.h>
#include <netdb.h>
#include "pplivePlayer.h"
#include "gmlive.h"
#include "ec_throw.h"
#include "scope_gruard.h"
#include <gmplayer.h>

PpLivePlayer* PpLivePlayer::self = NULL;

PpLivePlayer* PpLivePlayer::create(const std::string& stream_)
{
	if (!self)
		self = new PpLivePlayer(stream_);
	return self;
}

PpLivePlayer::PpLivePlayer(const std::string& stream_) : 
	stream(stream_),
	pplive_pid(-1),
	sop_sock(-1),
	gmplayer(NULL),
	is_running(false)
{
}

PpLivePlayer::~PpLivePlayer()
{
	stop();
	close(sop_sock);
	printf("pplive exit\n");
	self = NULL;
}

void PpLivePlayer::start(GMplayer& gmp)
{
	gmplayer = &gmp;
	if (is_running) {
		gmplayer->start(PPLIVESTREAM);
		return;
	}

	is_running = true;
	gmplayer = &gmp;
	extern char **environ;
	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);
		chdir("/tmp");
		const char* argv[5];

		argv[0] = "xpplive";
		argv[1] = stream.c_str();
		argv[2] = NULL;

		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		execvp(argv[0], (char* const *)argv);
		perror("xpplive execvp:");
		exit(127);
	} 

	pplive_pid = pid;
	printf("pplive_pid = %d\n",pplive_pid);

	signal_status_.emit(0);

	std::string& delay = GMConf["pplive_delay_time"];
	int idelay = atoi(delay.c_str());
	sop_time_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &PpLivePlayer::on_sop_time_status), idelay * 1000);

}

bool PpLivePlayer::on_sop_time_status()
{
	std::string& cache = GMConf["sopcast_mplayer_cache"];
	int icache = atoi(cache.c_str());
	icache = icache > 64 ? icache : 64;

	gmplayer->set_cache(icache);
	gmplayer->start(PPLIVESTREAM);

	return false;
}

void PpLivePlayer::stop()
{
	if (pplive_pid > 0) {
		for (;;) {
			kill(-pplive_pid, SIGKILL);
			int ret = (waitpid( -pplive_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}
		pplive_pid = -1;
	}

}

