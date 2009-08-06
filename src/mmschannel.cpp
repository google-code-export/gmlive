/*
 * =====================================================================================
 *
 *       Filename:  mmschannel.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月02日 09时16分15秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include "gmlive.h"
#include "mmschannel.h"
#include "MainWindow.h"
#include "mmsLivePlayer.h"

MMSChannel::MMSChannel(MainWindow* parent_):Channel(parent_)
					    ,wget_pid(-1)
					    ,refresh(false)
{
	init();
}

LivePlayer* MMSChannel::get_player(const std::string& stream,TypeChannel page)
{
	return MmsLivePlayer::create(stream);
}

void MMSChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/mms.lst",homedir);
	std::ifstream file(buf);
	if(!file){
		snprintf(buf,512,"%s/mms.lst",DATA_DIR);
		file.open(buf);
	}
	std::string line;
	std::string name;
	std::string stream;
	std::string groupname;
	std::string last;
	int users=0;
	m_liststore->clear(); //先清空列表
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			last = line.substr(pos+1,std::string::npos);

			pos = last.find_first_of("#");
			if(pos == std::string::npos)
				continue;
			stream = last.substr(0,pos);
			groupname = last.substr(pos+1,std::string::npos);
			addLine(users,name,stream,groupname);
		}
	}

	file.close();

}

void MMSChannel::addLine(int num, const Glib::ustring & name,const std::string& stream,const Glib::ustring& groupname)
{
	Gtk::TreeModel::Children children = m_liststore->children();
	Gtk::TreeModel::iterator listiter;
	listiter = getListIter(children,groupname);
	if(listiter == children.end())
		listiter = addGroup(groupname);

	Gtk::TreeModel::iterator iter = m_liststore->append(listiter->children());
	(*iter)[columns.users] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream;
	(*iter)[columns.type]=MMS_CHANNEL;

}
void MMSChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}

	char buf[512];
	char buf2[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/mms.lst.tmp",homedir);
	snprintf(buf2, 512,"%s/.gmlive/mms.lst",homedir);

	if (rename(buf, buf2))
		return;

	init();
	signal_stop_refresh_.emit();
}



void MMSChannel::refresh_list()
{
	if (refresh)
		return;
	refresh = true;
	signal_start_refresh_.emit();

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/mms.lst.tmp",homedir);

		const char* argv[6];
		argv[0] = "wget";
		//argv[1] = "http://www.gooth.cn/mms.lst";
		argv[1] = GMConf["mms_channel_url"].c_str();
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

		execvp("wget", (char* const *)argv);
		perror("wget execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &MMSChannel::wait_wget_exit), pid);

}
