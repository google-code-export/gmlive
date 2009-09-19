/*
 * =====================================================================================
 *
 *       Filename:  bookmarkchannel.cpp
 *
 *    Description:  bookmark for channel list
 *
 *        Version:  1.0
 *        Created:  2007年12月03日 00时44分49秒 CST
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
#include "bookmarkchannel.h"
#include "MainWindow.h"
#include "mmsLivePlayer.h"
#include "sopcastLivePlayer.h"
#include "pplivePlayer.h"
#include "ppsPlayer.h"

BookMarkChannel::BookMarkChannel(MainWindow* parent_):Channel( parent_)
{
	init();
}
LivePlayer* BookMarkChannel::get_player(const std::string& stream,TypeChannel page)
{
	switch( page ) {
		case MMS_CHANNEL:
			return MmsLivePlayer::create(stream);
		case PPLIVE_CHANNEL:
			return PpLivePlayer::create(stream);
		case SOPCAST_CHANNEL:
			return SopcastLivePlayer::create(stream);
		case PPS_CHANNEL:
			return PPSPlayer::create(stream);
		default:
			g_warn_if_reached();
			return NULL;
	}
}


void BookMarkChannel::init()
{
	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/bookmark.lst",homedir.c_str());
	std::ifstream file(buf);
	if(!file){
		//printf("buf is %s\n",buf);
		//std::cout<<"file error\n";
		return;
	}
	std::string line;
	std::string sec;
	std::string last;
	std::string name;
	std::string stream;
	std::string type;
	std::string group;
	std::string trimstring = "\t";
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,line.find_first_of(trimstring));
			sec = line.substr(pos+1,std::string::npos);

			pos = sec.find_first_of("#");
			if(pos==std::string::npos)
				continue;

			stream=sec.substr(0,sec.find_first_of(trimstring));
			last = sec.substr(pos+1,std::string::npos);

			pos = last.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			group=last.substr(0,last.find_first_of(trimstring));
			type= last.substr(pos+1,std::string::npos);

			//addLine(group,name,stream,type);
			addLine(name,stream,type,group);
		}
	}

	file.close();

}


void  BookMarkChannel::addLine(const Glib::ustring& name,const std::string& stream_,const Glib::ustring& type,const Glib::ustring& groupname)
{

	Gtk::TreeModel::Children children = m_liststore->children();
	Gtk::TreeModel::iterator listiter;
	listiter = getListIter(children,groupname);
	if(listiter == children.end())
		listiter = addGroup(groupname);

	TypeChannel type_;
	if("mms"==type)
		type_ = MMS_CHANNEL;
	else if("pplive" == type)
	{
		if(!parent->support_pplive())
			return;
		else
			type_ = PPLIVE_CHANNEL;
	}
	else if("sopcast" == type)
	{
		if(!parent->support_sopcast())
			return;
		else
			type_ = SOPCAST_CHANNEL;
	}
	else if("ppstream" == type)
	{
		type_ = PPS_CHANNEL;

	}
	else
		type_ = NONE;

	//Gtk::TreeModel::iterator iter = m_liststore->prepend();
	Gtk::TreeModel::iterator iter = m_liststore->prepend(listiter->children());
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream_;
	(*iter)[columns.type]=type_ ;

}
void BookMarkChannel::saveLine(const Glib::ustring & name,const std::string& stream_,TypeChannel type,const Glib::ustring&groupname_)
{

	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/bookmark.lst",homedir.c_str());
	std::ofstream file(buf,std::fstream::app);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}

	std::string stream;

	std::string strtype;
	switch (type) {
		case MMS_CHANNEL:
			strtype = "mms";
			break;
		case SOPCAST_CHANNEL:
			strtype = "sopcast";
			break;
		case PPLIVE_CHANNEL:
			strtype = "pplive";
			break;
		case PPS_CHANNEL:
			strtype = "ppstream";
			break;
	}
	stream = name +"\t#"+stream_+"\t#"+groupname_+"\t#"+strtype;
	file<<stream<<std::endl;
	file.close();
	addLine( name,stream_,strtype,groupname_);

}

