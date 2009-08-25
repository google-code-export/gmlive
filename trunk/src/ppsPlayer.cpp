/*
 * =====================================================================================
 *
 *       Filename:  ppsPlayer.cpp
 *
 *    Description:  pps播放器类
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
#include "ppsPlayer.h"
#include "gmlive.h"
#include "ec_throw.h"
#include "scope_gruard.h"
#include <gmplayer.h>

PPSPlayer* PPSPlayer::self = NULL;

PPSPlayer* PPSPlayer::create(const std::string& stream_)
{
	if (self && self->filename == stream_)
		return self;

	self = new PPSPlayer(stream_);
	return self;

}

PPSPlayer::PPSPlayer(const std::string& stream_) : 
	LivePlayer(stream_),
	pps_pid(-1),
	sop_sock(-1),
	gmplayer(NULL),
	is_running(false)
{
}

PPSPlayer::~PPSPlayer()
{
	stop();
	close(sop_sock);
	printf("pps exit\n");
	self = NULL;
}

void PPSPlayer::start(GMplayer& gmp)
{
	gmplayer = &gmp;
	if (is_running) {
		gmplayer->start(PPSSTREAM);
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

		argv[0] = "nohup";
		argv[1] = "xpps";
		argv[2] = filename.c_str();
		argv[3] = NULL;

		printf("play cmd %s\n",argv);
		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		execvp(argv[0], (char* const *)argv);
		perror("xpps execvp:");
		exit(127);
	} 

	pps_pid = pid;
	printf("pps_pid = %d\n",pps_pid);

	signal_status_.emit(0);

	//std::string& delay = GMConf["pps_delay_time"];
	//int idelay = atoi(delay.c_str());
	int idelay = 9;
	sop_time_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &PPSPlayer::on_sop_time_status), idelay * 1000);

}

bool PPSPlayer::on_sop_time_status()
{
	//std::string& cache = GMConf["pps_mplayer_cache"];
	//int icache = atoi(cache.c_str());
	//icache = icache > 64 ? icache : 64;
	int icache=512;

	gmplayer->set_cache(icache);

	gmplayer->start(PPSSTREAM);

	return false;
}

void PPSPlayer::stop()
{
	if (pps_pid > 0) {
		for (;;) {
			kill(-pps_pid, SIGKILL);
			int ret = (waitpid( -pps_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}
		pps_pid = -1;
	}

}

