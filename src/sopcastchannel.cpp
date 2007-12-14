
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
	refresh_list();
}

LivePlayer* SopcastChannel::get_player(const std::string& stream,TypeChannel page)
{
	return new SopcastLivePlayer(stream);
}

void SopcastChannel::parse_channel
(Gtk::TreeModel::iterator& iter, xmlNode* a_node)
{

	xmlChar* id = xmlGetProp(a_node, (const xmlChar*)"id");
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
	
	init();
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
		perror("wget execvp:");
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
	
	m_liststore->clear();
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

