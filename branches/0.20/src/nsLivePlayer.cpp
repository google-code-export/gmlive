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

NsLivePlayer* NsLivePlayer::self = NULL;
NsLivePlayer* NsLivePlayer::create(const std::string& id_)
{
	if (!self)
		self = new NsLivePlayer(id_);
	return self;
}

NsLivePlayer::NsLivePlayer(const std::string& id_) : 
	id(id_),
	ns_pid(-1),
	gmplayer(NULL),
	is_running(false)
{
}

NsLivePlayer::~NsLivePlayer()
{
	stop();
	printf("nslive exit\n");
	self = NULL;
}

void NsLivePlayer::start(GMplayer& gmp)
{
	gmplayer = &gmp;
	if (is_running)
		return;
	is_running = true;

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
	signal_status_.emit(0);

	std::string& delay_time = GMConf["nslive_delay_time"];
	int itime = atoi(delay_time.c_str());
	
	gmp_startup_time_conn =  Glib::signal_timeout().connect(sigc::mem_fun(*this, &NsLivePlayer::on_gmp_startup_time), itime * 1000);

}

void NsLivePlayer::stop()
{
	gmp_startup_time_conn.disconnect();
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

bool NsLivePlayer::on_gmp_startup_time()
{
	std::string& cache = GMConf["nslive_mplayer_cache"];
	int icache = atoi(cache.c_str());
	icache = icache < 64 ? 64 : icache;
	gmplayer->set_cache(icache);
	gmplayer->start(NSLIVESTREAM);
	return false;
}
