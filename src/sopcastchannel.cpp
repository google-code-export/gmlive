
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "sopcastchannel.h"
#include "MainWindow.h"


SopcastChannel::SopcastChannel(MainWindow* parent_):parent( parent_)
{
}

void SopcastChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/sopcast.lst",homedir);
	std::ifstream file(buf);
	if(!file){
		printf("buf is %s\n",buf);
		std::cout<<"file error\n";
		return;
	}
	std::string line;
	std::string name;
	std::string stream;
	int id=1;
	if(file){
		while(std::getline(file,line)){
			size_t pos = line.find_first_of("#");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			stream= line.substr(pos+1,std::string::npos);
			addLine(id,name,stream);
			id++;
		}
	}

	file.close();

}

void SopcastChannel::addLine(const int num, const Glib::ustring & name,const std::string& stream,const Glib::ustring& groupname)
{
	
	Gtk::TreeModel::Children children = m_liststore->children();
	Gtk::TreeModel::iterator listiter;
	listiter = getListIter(children,groupname);
	if(listiter == children.end())
		listiter = addGroup(groupname);

	Gtk::TreeModel::iterator iter = m_liststore->append(listiter->children());
	
	(*iter)[columns.id] = num;
	(*iter)[columns.name] = name;
	(*iter)[columns.freq] = 100;
	(*iter)[columns.stream]=stream;
	(*iter)[columns.type]=SOPCAST_CHANNEL;

}

void SopcastChannel::play_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	int channle_num = (*iter)[columns.id];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	parent->play(channle_num,stream,page);
	parent->getRecentChannel().saveLine(channle_num,name,stream,page);
}

void SopcastChannel::record_selection()
{

}
void SopcastChannel::store_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	int channle_num = (*iter)[columns.id];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	parent->getBookMarkChannel().saveLine(channle_num,name,stream,page);

}


bool SopcastChannel::on_button_press_event(GdkEventButton * ev)
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
		if(SOPCAST_CHANNEL == (*iter)[columns.type]){
			int channle_num = (*iter)[columns.id];
			Glib::ustring name = (*iter)[columns.name];
			std::string stream = (*iter)[columns.stream];
			parent->play(channle_num,stream,SOPCAST_CHANNEL);
			parent->getRecentChannel().saveLine(channle_num,name,stream,SOPCAST_CHANNEL);
		}
		else if(GROUP_CHANNEL == (*iter)[columns.type]){
			if(this->row_expanded(path))
				this->collapse_row(path);
			else{
				this->expand_row(path,false);
				this->scroll_to_row(path);
			}


	} else if ((ev->type == GDK_BUTTON_PRESS)
		   && (ev->button == 3)) {
		if(GROUP_CHANNEL != (*iter)[columns.type])
			parent->getMenu().popup(1,ev->time);
	}

}
