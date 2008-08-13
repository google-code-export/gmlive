/*
 * =====================================================================================
 *
 *       Filename:  playStream.cpp
 *
 *    Description:  播放网络流
 *
 *        Version:  1.0
 *        Created:  2007年12月22日 21时05分47秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#include "playStream.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "ec_throw.h"
#include <cassert>
#include "gmlive.h"
#include <string>
#include <list>


PlayStream::PlayStream()
	:xid(-1)
	,is_embed(true)
       ,is_oplayer(false)
{
}


PlayStream::~PlayStream()
{
	stop();
}


void PlayStream::initialize()
{
	signal_start().emit();

	Glib::RefPtr<Gdk::Window> gwin = this->get_window();

	std::list<std::string> pars;
	std::list<std::string>::iterator iter;
	if(is_oplayer){  //other player handle
		std::string& oplayer_cmd = GMConf["other_player_cmd"];
		if(!oplayer_cmd.empty()){
		size_t pose1 = 0;
		size_t pose2 = 0;
		for(;;) {
			pose2 = oplayer_cmd.find_first_of(" \t", pose1);
			pars.push_back(oplayer_cmd.substr(pose1, pose2 - pose1));
			if (pose2 == std::string::npos)
				break;
			pose1 = pose2 + 1;
		}
	}
		else
		{
		printf("%s:%d\n",__func__,__LINE__);
			//handle here
			return;
		}

	int argv_len = 512 + pars.size();
	printf("%s:%d :argv_len = %d\n",__func__,__LINE__,argv_len);
	const char* argv[argv_len];
	int i = 0;
	iter = pars.begin();
	for (; i < argv_len && iter != pars.end(); i++, ++iter) {
		argv[i] = (*iter).c_str();
		printf("argv[%d]=%s\n",i,argv[i]);
	}

	//strcpy((char*)argv[i],file.c_str());
	argv[i]=file.c_str();
	i++;
	argv[i] = NULL;

	my_system((char* const *) argv);

	}
	else{  //mplayer handle

	char cache_buf[32];
	snprintf(cache_buf, 32, "%d", cache);

	std::string& paramter = GMConf["mplayer_paramter"];
	//std::list<std::string> pars;
	if (!paramter.empty()) {
		size_t pos1 = 0;
		size_t pos2 = 0;
		for(;;) {
			pos2 = paramter.find_first_of(" \t", pos1);
			pars.push_back(paramter.substr(pos1, pos2 - pos1));
			if (pos2 == std::string::npos)
				break;
			pos1 = pos2 + 1;
		}
	}

	int argv_len = 10 + pars.size();
	const char* argv[argv_len];
	argv[0] = "mplayer";
	argv[1] = "-slave";
	argv[2] = "-idle";
	argv[3] = "-quiet";
	argv[4] = "-nomouseinput";
	argv[5] = "-noconsolecontrols";
	argv[6] = "-cache";
	argv[7] = cache_buf;
	//std::list<std::string>::iterator iter = pars.begin();
	iter = pars.begin();
	int i = 8;
	for (; i < argv_len && iter != pars.end(); i++, ++iter) {
		argv[i] = (*iter).c_str();
	}
	if (is_embed) {
		char wid_buf[32];
		snprintf(wid_buf, 32, "%d", this->get_id());
		printf("%s\n", wid_buf);
		argv[i++] = "-wid";
		argv[i++] = wid_buf;
	}
	argv[i] = NULL;

	my_system((char* const *) argv);
	}
}

void PlayStream::start()
{
	if (pausing())
		return pause();
	//if (running()||(is_oplayer))
	if (running())
		stop();
	printf("%s:%d\n",__func__,__LINE__);
	initialize();
	printf("%s:%d\n",__func__,__LINE__);
	char cb[256];
	int len = snprintf(cb, 256, "loadfile %s\n", file.c_str());
	cb[len] = 0;
	send_ctrl_command(cb);

}

void PlayStream::start(const std::string& filename)
{
	if (filename != file)
		file = filename;
	start();
}


void PlayStream::full_screen()
{
}


void PlayStream::set_embed(bool embed_)
{
	if (is_embed == embed_)
		return;
	is_embed = embed_;
	if (running()) 
		start();
}
void PlayStream::set_other_player(bool f_oplayer)
{
	is_oplayer = f_oplayer;

}

void PlayStream::on_mplayer_exit()
{
	signal_stop().emit();
}

