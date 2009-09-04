/*
 * =====================================================================================
 *
 *       Filename:  pplivechannel.cpp
 *
 *    Description:  pplive的列表支持
 *
 *        Version:  1.0
 *        Created:  2009年08月06日 14时41分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

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
#include "pplivechannel.h"
#include "MainWindow.h"
#include "scope_gruard.h"
#include "pplivePlayer.h"

xmlNode* get_pplive_address(xmlNode* a_node,
		Glib::ustring& str)
{
	if (!a_node)
		return NULL;

	//while (xmlStrcmp(a_node->name, (
}

Glib::ustring get_channel_item(xmlNode* a_node,
	       	const xmlChar* name)
{
	Glib::ustring str;
	if (!a_node)
		return str;

	while (xmlStrcmp(a_node->name, name))
		a_node = a_node->next;
	if (a_node) {
		xmlChar* name = 
			xmlNodeListGetString(a_node->doc,
					a_node->xmlChildrenNode, 1);
		str.assign((const char*)name);	
		xmlFree(name);
	}
	return str;
}

Gtk::TreeModel::iterator PpliveChannel::get_group_iter(const char* name)
{
	Gtk::TreeModel::iterator iter = getListIter(m_liststore->children(), name); 
	if (iter == m_liststore->children().end())
		return addGroup(name);
	return iter;
}

void PpliveChannel::parse_channel(xmlNode* a_node)
{
	xmlNode* cur_node = a_node->children;
	if ((!cur_node) && (cur_node->type != XML_ELEMENT_NODE))
		return;

	Glib::ustring channel_name = get_channel_item(cur_node, (const xmlChar*)"ChannelName");
	Glib::ustring catalog = get_channel_item(cur_node, (const xmlChar*)"Catalog");
	Glib::ustring peer_count = get_channel_item(cur_node, (const xmlChar*)"PeerCount");
	Glib::ustring bitrate = get_channel_item(cur_node, (const xmlChar*)"Bitrate");
	Glib::ustring play_link = get_channel_item(cur_node, (const xmlChar*)"PlayLink");

	Gtk::TreeModel::iterator piter = get_group_iter(catalog.c_str());
	Gtk::TreeModel::iterator iter = getListIter(piter->children(), channel_name);
	if (iter != piter->children().end())
		return;

	iter = m_liststore->append(piter->children());

	(*iter)[columns.name] = channel_name;

	int users = atoi(peer_count.c_str());
	(*iter)[columns.users] = users;
	int users_total = (*piter)[columns.users] + users;
	(*piter)[columns.users] = users_total;

	int freq = atoi(bitrate.c_str());
	(*iter)[columns.freq] = freq;

	(*iter)[columns.stream] = play_link;
	
	(*iter)[columns.type]=PPLIVE_CHANNEL;
}


void PpliveChannel::parse_channels(xmlNode* a_node)
{
	for (xmlNode* cur_node = a_node->children;
			cur_node; cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE ) &&
				(!xmlStrcmp(cur_node->name, (const xmlChar*)"channel")))
			parse_channel(cur_node);
	}
}

bool PpliveChannel::read_channels(const char* filename)
{
	xmlDoc* doc = xmlReadFile(filename, NULL, 0); 
	if (!doc) {
		char command[1024];
		snprintf(command, 1024, "iconv -f gb18030 -t utf8 %s | sed 's/encoding=\"gb2312\"/encoding=\"utf-8\"/' > /tmp/gmlive.xml.encode", filename);
		printf("%s\n", command);
		system(command);
		doc= xmlReadFile("/tmp/gmlive.xml.encode", NULL, 0);
		if (!doc) {
			std::cerr << "pplive file error: " << filename << std::endl;
			return false;
		}
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	ScopeGuard a = MakeGuard(&xmlCleanupParser);

	if (!root_element) {
		std::cerr << "pplive file is empty:" << filename << std::endl;
		return false;
	}

	parse_channels(root_element);
	return true;
}

bool PpliveChannel::init()
{
	char buf[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512, "%s/gmlive/pplive.lst", homedir.c_str());

	return read_channels(buf);
}

LivePlayer* PpliveChannel::get_player(const std::string& stream, TypeChannel page)
{
	return PpLivePlayer::create(stream);
}

void PpliveChannel::start_wget(const char* url)
{
	if (!url){
		signal_stop_refresh_.emit();
		refresh = false;
		return;
	}

	std::cout<< url << std::endl;

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		std::string homedir=Glib::get_user_config_dir();
		snprintf(buf, 512,"%s/gmlive/pplive.lst.tmp",homedir.c_str());

		const char* argv[6];
		argv[0] = "wget";
		//argv[1] = "http://list.pplive.com/zh-cn/xml/new.xml";
		argv[1] = url;
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

		execvp("wget", (char* const *)argv);
		perror("wget execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &PpliveChannel::wait_wget_exit), pid);
}

void PpliveChannel::refresh_list()
{
	if (refresh)
		return;

	m_liststore->clear();

	refresh = true;
	signal_start_refresh_.emit();

	const std::string& url =  GMConf["pplive_channel_url"];

	memcpy(url_buf, url.c_str(), MIN(url.size(), 1024));
	url_buf_cur = strtok(url_buf, " ");
	start_wget(url_buf_cur);
}

void PpliveChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}

	char buf[512];
	char buf2[512];
	std::string homedir=Glib::get_user_config_dir();
	snprintf(buf, 512,"%s/gmlive/pplive.lst.tmp",homedir.c_str());
	snprintf(buf2, 512,"%s/gmlive/pplive.lst",homedir.c_str());

	rename(buf, buf2);
	while (!init()) {
		Glib::ustring filename = user_select_list(
				_("open the pplive channel error,will you select a local file to instead the broken channel list?"));
		if (filename.empty())
			break;
		else {
			char command[512];
			snprintf(command, 512, "cp \"%s\" \"%s\"", filename.c_str(), buf2);
			system(command);
		}
	}

	url_buf_cur = strtok(NULL, " ");
	start_wget(url_buf_cur);
}


PpliveChannel::PpliveChannel(MainWindow* parent_) :
	Channel( parent_)
	,wget_pid(-1)
	,refresh(false)
{
	init();
}
