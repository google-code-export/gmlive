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

	char cache_buf[32];
	snprintf(cache_buf, 32, "%d", cache);

	std::string& paramter = GMConf["mplayer_paramter"];
	std::list<std::string> pars;
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
	argv[4] = "-cache";
	argv[5] = cache_buf;
	std::list<std::string>::iterator iter = pars.begin();
	int i = 6;
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

void PlayStream::start()
{
	if (pausing())
		return pause();
	if (running())
		stop();
	initialize();
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

void PlayStream::on_mplayer_exit()
{
	signal_stop().emit();
}

