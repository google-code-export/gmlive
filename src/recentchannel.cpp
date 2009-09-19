/*
 * =====================================================================================
 *
 *       Filename:  recentchannel.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月02日 20时19分58秒 CST
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
#include <vector>
#include "recentchannel.h"
#include "MainWindow.h"
#include "mmsLivePlayer.h"
#include "sopcastLivePlayer.h"
#include "pplivePlayer.h"
#include "ppsPlayer.h"

RecentChannel::RecentChannel(MainWindow* parent_):Channel( parent_)
{
	init();
}

RecentChannel::~RecentChannel()
{}

LivePlayer* RecentChannel::get_player( const std::string& stream,TypeChannel page)
{
	switch(page) {
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
void RecentChannel::init()
{
	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/recent.lst",homedir.c_str());
	std::ifstream file(buf);
	if(!file){
		std::ofstream out(buf);
		out.close();
		file.open(buf);
		//std::cout<<"file error\n";
		//return;
	}
	std::string line;
	std::string last;
	std::string name;
	std::string stream;
	std::string type;
	std::string trimstring = "\t";
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			last = line.substr(pos+1,std::string::npos);

			pos = last.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			//stream=last.substr(0,pos);
			stream=last.substr(0,last.find_first_of(trimstring));
			type= last.substr(pos+1,std::string::npos);
			
			int id=0;
			addLine(id,name,stream,type);
		}
	}

	file.close();

}

void  RecentChannel::addLine(int num,const Glib::ustring& name,const std::string& stream_,const Glib::ustring& type)
{
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

	Gtk::TreeModel::iterator iter = m_liststore->prepend();
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream_;


	(*iter)[columns.type]=type_ ;

}

void RecentChannel::saveLine(const Glib::ustring & name,const std::string& stream_,TypeChannel type)
{

	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/recent.lst",homedir.c_str());
	std::ifstream file(buf);
	std::vector<std::string> list;
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	std::string line;
	int num=0;
	if(file){
		while(std::getline(file,line)){
			list.push_back(line);
			num++;
			if(25==num)
				break;
		}
	}
	file.close();
	std::string stream;
	
	std::string strtype;
	if(type == MMS_CHANNEL)
	{
		//stream = name +"\t#"+stream_+"\t#mms";
		strtype = "mms";
	}
	else if (type == SOPCAST_CHANNEL)
	{
		//stream = name +"\t#"+stream_+"\t#sopcast";
		strtype = "sopcast";
	}
	else if(type == PPLIVE_CHANNEL)
	{
		//stream = name +"\t#"+stream_+"\t#pplive";
		strtype = "pplive";
	}
	else if(type ==PPS_CHANNEL)
	{
		//stream = name +"\t#"+stream_+"\t#ppstream";
		strtype = "ppstream";

	}
	stream = name +"\t#"+stream_+"\t#"+strtype;
	std::vector<std::string>::iterator iter = std::find(list.begin(),list.end(),stream);
	if(iter == list.end())
	{
		list.push_back(stream);
		std::ofstream outfile(buf);
		for(iter=list.begin();iter!=list.end();++iter)
		{
			if(iter == list.begin()&&(num==10))
				;
			else
				outfile<<*iter<<std::endl;
		}
		outfile.close();

		int users  =0;
		addLine(users, name,stream_,strtype);
	}

}

