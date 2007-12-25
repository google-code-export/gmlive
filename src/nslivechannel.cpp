/*
 * =====================================================================================
 *
 *       Filename:  nslivechannel.cpp
 *
 *    Description:  support nslive channel list
 *
 *        Version:  1.0
 *        Created:  2007年12月02日 09时00分39秒 CST
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
#include <sys/stat.h>
#include <sys/wait.h>
#include "nslivechannel.h"
#include "MainWindow.h"
#include "nsLivePlayer.h"
#include "CodeConverter.h"
#include <sys/stat.h>
#include <sys/types.h>


NSLiveChannel::NSLiveChannel(MainWindow* parent_):Channel( parent_)
	 ,genlist_pid(-1)
	 ,refresh(false)
{
	refresh_list();
	//init();
}

NSLiveChannel::~NSLiveChannel()
{
	system("killall nsweb>/dev/null 2>&1");
}

LivePlayer* NSLiveChannel::get_player(const std::string& stream,TypeChannel page) 
{
	return new NsLivePlayer(stream);
}

void NSLiveChannel::init()
{

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.ulive/prog.list",homedir);
	//printf("%s\n", buf);


	m_liststore->clear();

	std::ifstream file(buf);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	CodeConverter code("gbk","utf-8");
	std::string gbkline;
	std::string line;
	std::string last;
	std::string name;
	std::string users;
	std::string stream(NSLIVESTREAM);
	std::string groupname;
	int num;
	if(file){
		while(std::getline(file,gbkline)){
			line = code.convert(gbkline);
			//get stream
			size_t pos = line.find_first_of(",");
			if(pos==std::string::npos)
				continue;
			stream = line.substr(0,pos);
			//std::cout<<"stream "<<stream<<std::endl;
			last = line.substr(pos+1,std::string::npos);

			//get name
			pos = last.find_first_of(",");
			if(pos == std::string::npos)
				continue;
			name = last.substr(0,pos);
			//std::cout<<"name "<<name<<std::endl;
			line  = last.substr(pos+1,std::string::npos);

			//get groupname
			pos = line.find_first_of(",");
			if(pos == std::string::npos)
				continue;
			groupname= line.substr(0,pos);
			//std::cout<<"groupname "<<groupname<<std::endl;
			 last = line.substr(pos+1,std::string::npos);

			 //get users
			pos = last.find_first_of(",");
			if(pos == std::string::npos)
				continue;
			users=last.substr(pos+1,std::string::npos);

			//std::cout<<"users "<<users<<std::endl;

			num=atoi(users.c_str());
			addLine(num,name,stream,groupname);
		}
	}

	file.close();

}


void NSLiveChannel::addLine(const int num, const Glib::ustring & name,const std::string& stream,const Glib::ustring& groupname)
{
	Gtk::TreeModel::Children children = m_liststore->children();
	Gtk::TreeModel::iterator listiter;
	listiter = getListIter(children,groupname);
	if(listiter == children.end())
		listiter = addGroup(groupname);
	int total_num = (*listiter)[columns.users];
	total_num += num;
	(*listiter)[columns.users] = total_num;

	Gtk::TreeModel::iterator iter = m_liststore->append(listiter->children());
	(*iter)[columns.users] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 350;
	(*iter)[columns.stream]=stream;
	(*iter)[columns.type]=NSLIVE_CHANNEL;

}

void NSLiveChannel::wait_pid_exit(GPid pid, int)
{
	if (genlist_pid != -1) {
		waitpid(genlist_pid, NULL, 0);
		genlist_pid = -1;

		refresh = false;
	}
	
	init();
}

void NSLiveChannel::refresh_list()
{
	if (refresh)
		return;
	refresh = true;

	system("killall nesweb >/dev/null 2>&1");

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);

		execlp("nsweb", "nsweb", NULL);
		perror("nsweb  execvl:");
		exit(127);
	} 
	genlist_pid = pid;
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &NSLiveChannel::wait_pid_exit), pid);

}


