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
#include "nslivechannel.h"
#include "MainWindow.h"


NSLiveChannel::NSLiveChannel(MainWindow* parent_):parent( parent_)
{


}
void NSLiveChannel::init()
{

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/nslive.lst",homedir);
	std::ifstream file(buf);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	std::string line;
	std::string name;
	std::string uid;
	std::string stream(NSLIVESTREAM);
	int id;
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			uid = line.substr(pos+1,std::string::npos);
			id = atoi(uid.c_str());
			addLine(id,name,stream);
		}
	}

	file.close();

}

void NSLiveChannel::addLine(const int num, const Glib::ustring & name,const std::string& stream)
{
	Gtk::TreeModel::iterator iter = m_liststore->append();
	(*iter)[columns.id] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream;
	(*iter)[columns.type]=NSLIVE_CHANNEL;

}

bool NSLiveChannel::on_button_press_event(GdkEventButton * ev)
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
		int channle_num = (*iter)[columns.id];
		printf("双击事件%d \n",channle_num);
		parent->nslive_play(channle_num);

	} else if ((ev->type == GDK_BUTTON_PRESS)
		   && (ev->button == 3)) {
	}

}
