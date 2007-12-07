
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "sopcastchannel.h"
#include "MainWindow.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "sopcast_live_player.h"
#include <sys/types.h>
#include <sys/wait.h>


xmlNode* get_sop_address(xmlNode* a_node,
	       	Glib::ustring& str)
{
	if (!a_node)
		return NULL;

	while (xmlStrcmp(a_node->name, (const xmlChar*)"sop_address"))
		a_node = a_node->next;
	if (a_node) {
		xmlNode* cur_node = a_node->children;
		xmlChar* value = 
			xmlNodeListGetString(cur_node->doc,
					cur_node->xmlChildrenNode, 1);
		str.assign((const char*)value);	
		xmlFree(value);
	}
	return a_node;
}



xmlNode* get_channel_item(xmlNode* a_node,
	       	const xmlChar* name, Glib::ustring& str)
{
	if (!a_node)
		return NULL;

	while (xmlStrcmp(a_node->name, name))
		a_node = a_node->next;
	if (a_node) {
		xmlChar* name = 
			xmlNodeListGetString(a_node->doc,
					a_node->xmlChildrenNode, 1);
		str.assign((const char*)name);	
		xmlFree(name);
	}
	return a_node;
}

SopcastChannel::SopcastChannel(MainWindow* parent_)
	:Channel( parent_)
	 ,wget_pid(-1)
	 ,refresh(false)
{
	init();
}

LivePlayer* SopcastChannel::get_player(GMplayer& gmp, const std::string& stream)
{
	return new SopcastLivePlayer(gmp, stream);
}

void SopcastChannel::parse_channel
(Gtk::TreeModel::iterator& iter, xmlNode* a_node)
{

	xmlChar* id = xmlGetProp(a_node, (const xmlChar*)"id");
	//(*iter)[columns.id] = id;
	xmlFree(id);
	Glib::ustring str;
	xmlNode* cur_node = a_node->children;
	if (cur_node->type != XML_ELEMENT_NODE) 
		return;
	cur_node = get_channel_item(cur_node, (const xmlChar*)"name", str);
	(*iter)[columns.name] = str;

	cur_node = get_channel_item(cur_node, (const xmlChar*)"user_count", str);
	int users = atoi(str.c_str());
	(*iter)[columns.users] = users;

	cur_node = get_channel_item(cur_node, (const xmlChar*)"kbps", str);
	int freq = atoi(str.c_str());
	(*iter)[columns.freq] = freq;

	cur_node = get_sop_address(cur_node, str);
	(*iter)[columns.stream] = str;

	(*iter)[columns.type]=SOPCAST_CHANNEL;
}

void SopcastChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}
	
	signal_stop_refresh_.emit();
}
void SopcastChannel::refresh_list()
{
	if (refresh)
		return;
	refresh = true;
	signal_start_refresh_.emit();

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/sopcast.lst",homedir);

		const char* argv[5];
       		argv[0] = "wget";
		argv[1] = "http://www.sopcast.com/gchlxml";
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = NULL;

		execvp("wget", (char* const *)argv);
		perror("nslive execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &SopcastChannel::wait_wget_exit), pid);

}

void SopcastChannel::parse_group(xmlNode* a_node)
{
	xmlChar* name = xmlNodeListGetString(a_node->doc,
			a_node->xmlChildrenNode, 1);
	Gtk::TreeModel::iterator iter = addGroup((const char*)name);
	xmlFree(name);

	for (xmlNode* cur_node = a_node->children; cur_node; 
			cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE) && 
				(!xmlStrcmp(cur_node->name, 
					    (const xmlChar*)"channel"))) {
			Gtk::TreeModel::iterator citer = m_liststore->append(iter->children());
			parse_channel(citer, cur_node);
		}
	}
}

void SopcastChannel::parse_channels(xmlNode* a_node)
{
	for (xmlNode* cur_node = a_node->children;
		       	cur_node; cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE) &&
				(!xmlStrcmp(cur_node->name,
					    (const xmlChar*)"group")))
			parse_group(cur_node);
	}
}
					    

void SopcastChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/sopcast.lst",homedir);
	
	xmlDoc* doc = xmlReadFile(buf, NULL, 0);
	if (!doc) {
		std::cout <<"file error: " << buf << std::endl;
		return;
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	if (!root_element) {
		xmlCleanupParser();
		std::cout << "file is empty\n";
		return;
	}

	parse_channels(root_element);
	
	xmlCleanupParser();
	
}

//void SopcastChannel::addLine(const int num, const Glib::ustring & name,const std::string& stream,const Glib::ustring& groupname)
//{
//	
//	Gtk::TreeModel::Children children = m_liststore->children();
//	Gtk::TreeModel::iterator listiter;
//	listiter = getListIter(children,groupname);
//	if(listiter == children.end())
//		listiter = addGroup(groupname);
//
//	Gtk::TreeModel::iterator iter = m_liststore->append(listiter->children());
//	
//	(*iter)[columns.users] = num;
//	(*iter)[columns.name] = name;
//	(*iter)[columns.freq] = 100;
//	(*iter)[columns.stream]=stream;
//	(*iter)[columns.type]=SOPCAST_CHANNEL;
//
//}

void SopcastChannel::play_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	parent->play(stream,this);
	parent->getRecentChannel().saveLine(name,stream,page);
}

void SopcastChannel::record_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	parent->record(stream,this);

}
void SopcastChannel::store_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	int channle_num = (*iter)[columns.users];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	parent->getBookMarkChannel().saveLine(name,stream,page);

}


//bool SopcastChannel::on_button_press_event(GdkEventButton * ev)
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
//		if(SOPCAST_CHANNEL == (*iter)[columns.type]){
//			Glib::ustring name = (*iter)[columns.name];
//			std::string stream = (*iter)[columns.stream];
//			parent->play(stream,SOPCAST_CHANNEL);
//			parent->getRecentChannel().saveLine(name,stream,SOPCAST_CHANNEL);
//		}
//		else if(GROUP_CHANNEL == (*iter)[columns.type]){
//			if(this->row_expanded(path))
//				this->collapse_row(path);
//			else{
//				this->expand_row(path,false);
//				this->scroll_to_row(path);
//			}
//		}
//
//
//	} else if ((ev->type == GDK_BUTTON_PRESS)
//		   && (ev->button == 3)) {
//		if(GROUP_CHANNEL != (*iter)[columns.type])
//			parent->getMenu().popup(1,ev->time);
//	}
//
//}
