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

RecentChannel::RecentChannel(MainWindow* parent_):parent( parent_)
{
}

RecentChannel::~RecentChannel()
{}
void RecentChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/recent.lst",homedir);
	std::ifstream file(buf);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	std::string line;
	std::string name;
	std::string stream;
	std::string type;
	int id;
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			size_t pos2 = line.find_first_of(";");
			if(pos2==std::string::npos)
				continue;
			type= line.substr(pos2+1,std::string::npos);
			if("mms"==type)
			{
				id=0;
				stream=line.substr(pos+1,pos2);
				addLine(id,name,stream);
			}
			else if("nslive" == type)
			{
				stream=line.substr(pos+1,pos2);
				id = atoi(stream.c_str());
				std::string stream_(NSLIVESTREAM);
				addLine(id,name,stream_);
			}
		}
	}

	file.close();

}

void  RecentChannel::addLine(const int num,const Glib::ustring& name,const std::string& stream_,const Glib::ustring& groupname)
//void RecentChannel::addLine(const int num, const Glib::ustring & name,const std::string& stream_)
{
	Gtk::TreeModel::iterator iter = m_liststore->prepend();
	(*iter)[columns.id] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream_;
	if(0 == num)
		(*iter)[columns.type]=MMS_CHANNEL;
	else
		(*iter)[columns.type]=NSLIVE_CHANNEL;

}

void RecentChannel::saveLine(const int id, const Glib::ustring & name,const std::string& stream_)
{

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/recent.lst",homedir);
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
			if(10==num)
				break;
		}
	}
	file.close();
	std::string stream;
	if(0==id)
		stream = name +"\t#"+stream_+"\t;mms";
	else
	{
		char b[12];
		sprintf(b,"%d",id);
		std::string str(b);
		stream = name +"\t#"+str+"\t;nslive";
	}
	std::vector<std::string>::iterator iter = std::find(list.begin(),list.end(),stream);
	if(iter == list.end())
	{
		list.push_back(stream);
		std::ofstream outfile(buf);
		//std::vector<std::string>::iterator iter = list.begin();
		for(iter=list.begin();iter!=list.end();iter++)
		{
			if(iter == list.begin()&&(num==10))
				;
			else
				outfile<<*iter<<std::endl;
		}
		outfile.close();

		addLine(id, name,stream_);
	}

}
bool RecentChannel::on_button_press_event(GdkEventButton * ev)
{
	bool result = Gtk::TreeView::on_button_press_event(ev);

	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return result;

	Gtk::TreeModel::Path path(iter);
	Gtk::TreeViewColumn * tvc;
	int cx, cy;
	/** get_path_at_pos() 是为确认鼠标是否在选择行上点击的*/
	if (!this->
			get_path_at_pos((int) ev->x, (int) ev->y, path, tvc, cx, cy))
		return FALSE;
	if ((ev->type == GDK_2BUTTON_PRESS ||
				ev->type == GDK_3BUTTON_PRESS)) {
		std::string stream = (*iter)[columns.stream];
		int id = (*iter)[columns.id];

		if(MMS_CHANNEL == (*iter)[columns.type])
			parent->mms_play(stream);
		else if(NSLIVE_CHANNEL == (*iter)[columns.type])
			parent->nslive_play(id);

	} else if ((ev->type == GDK_BUTTON_PRESS)
			&& (ev->button == 3)) {
	}

}
