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
#include <glib.h>
#include <glib/gstdio.h>
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
		//close(STDOUT_FILENO);
		chdir("/tmp");
		const gchar* cmdv[]={"ipcrm","-Q","0x00000fb5","-Q","0x00000908",NULL,NULL};
		g_spawn_async(NULL, (gchar**)cmdv, NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL);

		g_setenv("LD_PRELOAD","libppswrapper-preload.so.0.0.0",TRUE);
		const char* argv[3];


		argv[0] = "xpps";
		argv[1] = filename.c_str();
		argv[2] = NULL;

		printf("play cmd %s\n",argv[0]);
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
	pps_time_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &PPSPlayer::on_pps_time_status), idelay * 1000);

}

int connect_to_server(const char *host, int portnum);
bool PPSPlayer::on_pps_time_status()
{
#if 0
	if (-1 == sop_sock) {
		try {
			sop_sock = connect_to_server("127.0.0.1", 8098);
			EC_THROW( -1 == write(sop_sock, "state\ns\n", sizeof("state\ns\n")));

			pps_sock_conn = Glib::signal_io().connect(
					sigc::mem_fun(*this, &PPSPlayer::on_pps_sock),
					sop_sock, Glib::IO_IN);	
		} catch (...)
		{}
	}

	sop_file = fdopen(sop_sock, "r");
	return true;
#endif
	/** 检测pps的文件大于cache时才开始播放*/
	struct stat statbuf;
	if(0== g_stat(PPSSTREAM, &statbuf)){

		if(statbuf.st_size >1024)
		{
			gmplayer->start(PPSSTREAM);
			return false;
		}
	}
	//return false;
	return true;
}

bool PPSPlayer::on_pps_sock(const Glib::IOCondition& condition)
{
	char *p = NULL;
	
	getline(&p, 0, sop_file);

	int i = atoi(p ? p : "0");
	free(p);

	if (i = 100){

		EC_THROW( -1 == write(sop_sock, "quit\ns\n", sizeof("quit\ns\n")));
		gmplayer->start(PPSSTREAM);

		pps_time_conn.disconnect(); // 启动mpaleyr，停掉显示缓冲状态
		fclose(sop_file);
		sop_file = NULL;
		close(sop_sock);	   // 关掉状态查询端口
		sop_sock = -1;
		return false;
	} else {
		EC_THROW( -1 == write(sop_sock, "state\ns\n", sizeof("state\ns\n")));
	}
	signal_status_.emit(i);
	return true;

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

