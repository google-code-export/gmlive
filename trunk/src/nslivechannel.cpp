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
#include <sys/wait.h>
#include "nslivechannel.h"
#include "MainWindow.h"
#include "ns_live_player.h"


NSLiveChannel::NSLiveChannel(MainWindow* parent_):Channel( parent_)
	 ,genlist_pid(-1)
	 ,refresh(false)
{
	char homepath[512];
	char* homedir = getenv("HOME");
	snprintf(homepath,512,"%s/.gmlive/",homedir);
	mkdir(homepath,S_IRUSR|S_IWUSR|S_IXUSR);
	refresh_list();
}

LivePlayer* NSLiveChannel::get_player(GMplayer& gmp, const std::string& stream,TypeChannel page) 
{
	int channel_num = atoi(stream.c_str());
	return new NsLivePlayer(gmp, channel_num);
}

void NSLiveChannel::init()
{

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/nslive.lst",homedir);


	m_liststore->clear();

	std::ifstream file(buf);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	std::string line;
	std::string last;
	std::string name;
	std::string users;
	std::string stream(NSLIVESTREAM);
	std::string groupname;
	int num;
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
			stream  = last.substr(0,pos);
			line  = last.substr(pos+1,std::string::npos);
			pos = line.find_first_of("#");
			if(pos == std::string::npos)
				continue;
			groupname= line.substr(0,pos);
			users = line.substr(pos+1,std::string::npos);
			
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

	Gtk::TreeModel::iterator iter = m_liststore->append(listiter->children());
	(*iter)[columns.users] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 350;
	(*iter)[columns.stream]=stream;
	(*iter)[columns.type]=NSLIVE_CHANNEL;

}

//void NSLiveChannel::play_selection()
//{
//	Glib::RefPtr < Gtk::TreeSelection > selection =
//	    this->get_selection();
//	Gtk::TreeModel::iterator iter = selection->get_selected();
//	if (!selection->count_selected_rows())
//		return ;
//	TypeChannel page = (*iter)[columns.type];
//	Glib::ustring name = (*iter)[columns.name];
//	std::string stream = (*iter)[columns.stream];
//
//	parent->play(stream,this);
//	parent->getRecentChannel().saveLine(name,stream,page);
//}
//
//void NSLiveChannel::record_selection()
//{
//	Glib::RefPtr < Gtk::TreeSelection > selection =
//	    this->get_selection();
//	Gtk::TreeModel::iterator iter = selection->get_selected();
//	if (!selection->count_selected_rows())
//		return ;
//	TypeChannel page = (*iter)[columns.type];
//	Glib::ustring name = (*iter)[columns.name];
//	std::string stream = (*iter)[columns.stream];
//
//	parent->record(stream,this);
//
//}
//void NSLiveChannel::store_selection()
//{
//	Glib::RefPtr < Gtk::TreeSelection > selection =
//	    this->get_selection();
//	Gtk::TreeModel::iterator iter = selection->get_selected();
//	if (!selection->count_selected_rows())
//		return ;
//	TypeChannel page = (*iter)[columns.type];
//	Glib::ustring name = (*iter)[columns.name];
//	std::string stream = (*iter)[columns.stream];
//
//	parent->getBookMarkChannel().saveLine(name,stream,page);
//
//}
		
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

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/nslive.lst",homedir);

		const char* argv[2];
       		argv[0] = "list";
		argv[1] = NULL;

		char cmd[512];
		snprintf(cmd,512,"sh -c %s/gennslist",DATA_DIR);
		//execv(cmd, (char* const* )argv);
		execvp("gennslist", (char* const* )argv);
		perror("gennslist  execvl:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &NSLiveChannel::wait_pid_exit), pid);

}


