
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gtkmm.h>
#include <gtkmm/dialog.h>
#include <glib/gi18n.h>
#include "sopcastchannel.h"
#include "MainWindow.h"
#include "sopcastLivePlayer.h"
#include "scope_gruard.h"


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
	//refresh_list();
}

LivePlayer* SopcastChannel::get_player(const std::string& stream,TypeChannel page)
{
	return SopcastLivePlayer::create(stream);
}

int SopcastChannel::parse_channel
(Gtk::TreeModel::iterator& iter, xmlNode* a_node)
{

	//xmlChar* id = xmlGetProp(a_node, (const xmlChar*)"id");
	//xmlFree(id);
	Glib::ustring str;
	xmlNode* cur_node = a_node->children;
	if ((!cur_node) && (cur_node->type != XML_ELEMENT_NODE))
		return 0;
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
	return users;
}

void SopcastChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}
	
	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	char buf2[512];
	snprintf(buf, 512,"%s/gmlive/sopcast.lst.tmp",homedir.c_str());
	snprintf(buf2, 512,"%s/gmlive/sopcast.lst",homedir.c_str());

	rename(buf, buf2);
	while (!init()) {
		Glib::ustring filename = user_select_list(
				_("open the sopcast channel error,will you select a local file to instead the broken channel list?"));
		if (filename.empty())
			break;
		else {
			char command[512];
			snprintf(command, 512, "cp \"%s\" \"%s\"", filename.c_str(), buf2);
			system(command);
		}
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
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		std::string homedir=Glib::get_user_config_dir();
		snprintf(buf, 512,"%s/gmlive/sopcast.lst.tmp",homedir.c_str());

		const char* argv[6];
       		argv[0] = "wget";
		//argv[1] = "http://www.sopcast.com/gchlxml";
		argv[1] = GMConf["sopcast_channel_url"].c_str();
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

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

	int user_total = 0;
	for (xmlNode* cur_node = a_node->children; cur_node; 
			cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE) && 
				(!xmlStrcmp(cur_node->name, 
					    (const xmlChar*)"channel"))) {
			Gtk::TreeModel::iterator citer = m_liststore->append(iter->children());
			user_total += parse_channel(citer, cur_node);
		}
	}
	(*iter)[columns.users] = user_total;
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
					    

bool SopcastChannel::init()
{
	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/sopcast.lst",homedir.c_str());
	//
	//如果读取默认列表正确则退出
	return read_channels(buf);
}
bool SopcastChannel::read_channels(const char* filename)
{
	m_liststore->clear();
	xmlDoc* doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		std::cerr <<"file error: " << filename << std::endl;
		return false;
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	ScopeGuard a = MakeGuard(&xmlCleanupParser);
	if (!root_element) {
		//xmlCleanupParser();
		std::cerr << "file is empty" << std::endl;
		return false;
	}

	parse_channels(root_element);
	
	//xmlCleanupParser();
	return true;
}


