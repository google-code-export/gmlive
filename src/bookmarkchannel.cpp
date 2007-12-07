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

BookMarkChannel::BookMarkChannel(MainWindow* parent_):Channel( parent_)
{
	init();
}
void BookMarkChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/bookmark.lst",homedir);
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
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			size_t pos2 = line.find_first_of("#");
			if(pos2==std::string::npos)
				continue;
			type= line.substr(pos2+1,std::string::npos);
				stream=line.substr(pos+1,pos2);
				/*
			if("mms"==type||"sopcast"==type)
			{
				stream=line.substr(pos+1,pos2);
			}
			else if("nslive" == type)
			{
				stream=line.substr(pos+1,pos2);
				id = atoi(stream.c_str());
				std::string stream_(NSLIVESTREAM);
			}
			else
				continue;
				*/
			int id=0;
			addLine(id,name,stream,type);
		}
	}

	file.close();

}


void  BookMarkChannel::addLine(const int num,const Glib::ustring& name,const std::string& stream_,const Glib::ustring& type)
{
	Gtk::TreeModel::iterator iter = m_liststore->prepend();
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream_;

	TypeChannel type_;
	if("mms"==type)
		type_ = MMS_CHANNEL;
	else if("nslive" == type)
		type_ = NSLIVE_CHANNEL;
	else if("sopcast" == type)
		type_ = SOPCAST_CHANNEL;
	else
		type_ = NONE;
	(*iter)[columns.type]=type_ ;

}
void BookMarkChannel::saveLine(const Glib::ustring & name,const std::string& stream_,TypeChannel type)
{

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/bookmark.lst",homedir);
	std::ofstream file(buf,std::fstream::app);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}

	std::string stream;

	std::string strtype;
	if(type == MMS_CHANNEL)
	{
		stream = name +"\t#"+stream_+"\t;mms";
		strtype = "mms";
	}
	else if (type == SOPCAST_CHANNEL)
	{
		stream = name +"\t#"+stream_+"\t;sopcast";
		strtype = "sopcast";
	}
	else
	{
		stream = name +"\t#"+stream_+"\t;nslive";
		strtype = "nslive";
	}
	file<<stream<<std::endl;
	file.close();
	int users = 0;
	addLine(users, name,stream_,strtype);

}

//bool BookMarkChannel::on_button_press_event(GdkEventButton * ev)
//{
//	bool result = Gtk::TreeView::on_button_press_event(ev);
//
//	Glib::RefPtr < Gtk::TreeSelection > selection =
//	    this->get_selection();
//	Gtk::TreeModel::iterator iter = selection->get_selected();
//	if (!selection->count_selected_rows())
//		return result;
//
//	Gtk::TreeModel::Path path(iter);
//	Gtk::TreeViewColumn * tvc;
//	int cx, cy;
//					/** get_path_at_pos() 是为确认鼠标是否在选择行上点击的*/
//	if (!this->
//	    get_path_at_pos((int) ev->x, (int) ev->y, path, tvc, cx, cy))
//		return FALSE;
//	if ((ev->type == GDK_2BUTTON_PRESS ||
//	     ev->type == GDK_3BUTTON_PRESS)) {
//		std::string stream = (*iter)[columns.stream];
//		Glib::ustring name = (*iter)[columns.name];
//		TypeChannel type_ = (*iter)[columns.type];
//		parent->play(stream,type_);
//		parent->getRecentChannel().saveLine(name,stream,type_);
//
//	} else if ((ev->type == GDK_BUTTON_PRESS)
//		   && (ev->button == 3)) {
//	}
//
//}
