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

	Gtk::TreeModel::iterator iter = get_group_iter(catalog.c_str());
	iter = m_liststore->append(iter->children());

	(*iter)[columns.name] = channel_name;

	int users = atoi(peer_count.c_str());
	(*iter)[columns.users] = users;

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
	m_liststore->clear();
	xmlDoc* doc = xmlReadFile(filename, NULL, 0); 
	if (!doc) {
		std::cerr << "pplive file error: " << filename << std::endl;
		return false;
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

void PpliveChannel::init()
{
	char buf[512];
	snprintf(buf, 512, "%s/.gmlive/pplive.lst", getenv("HOME") );

	if (read_channels(buf))
		return;

	//属于读取默认列表不正确的处理
	int result;
	std::string filename;
	Gtk::FileChooserDialog dialog(_("Please select a channel list file"),Gtk::FILE_CHOOSER_ACTION_OPEN);
	Gtk::MessageDialog askDialog(_("open channles error")
			,false
			,Gtk::MESSAGE_QUESTION
			,Gtk::BUTTONS_OK_CANCEL
			);
	askDialog.set_secondary_text(_("open the sopcast channel error,will you select a local file to instead the broken channel list?"));
	result = askDialog.run();
	switch(result){
		case(Gtk::RESPONSE_OK):
			//open a file select window
			int f_result;
			dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
			dialog.add_button(Gtk::Stock::OPEN,Gtk::RESPONSE_OK);
			f_result = dialog.run();
			switch(f_result){
				case(Gtk::RESPONSE_OK):
					filename = dialog.get_filename();
					int out;
					out = read_channels(filename.c_str());
					if(!out) //本地的频道列表打开错误处理
					{
					Gtk::MessageDialog warnDialog(_("File error"),
							false);
					warnDialog.set_secondary_text(_("the local channles list file has wrong formats"));
					warnDialog.run();
					}
					else
					{
						//这里处理把选择的文件替换成~/.gmlive/pplive.lst

					}

					break;
				case(Gtk::RESPONSE_CANCEL):
					break;
				default:
					break;
			}

			break;
		case(Gtk::RESPONSE_CANCEL):
			break;
		default:
			break;
	}
	return;

	

}

LivePlayer* PpliveChannel::get_player(const std::string& stream, TypeChannel page)
{
	return PpLivePlayer::create(stream);
}

void PpliveChannel::refresh_list()
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
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/pplive.lst.tmp",homedir);

		const char* argv[6];
       		argv[0] = "wget";
		argv[1] = "http://list.pplive.com/zh-cn/xml/new.xml";
		//argv[1] = GMConf["pplive_channel_url"].c_str();
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

void PpliveChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}
	
	char buf[512];
	char buf2[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/pplive.lst.tmp",homedir);
	snprintf(buf2, 512,"%s/.gmlive/pplive.lst",homedir);

	if (rename(buf, buf2))
		return;

	init();
	signal_stop_refresh_.emit();
}


PpliveChannel::PpliveChannel(MainWindow* parent_) :
	Channel( parent_)
	,wget_pid(-1)
	,refresh(false)
{
	init();
}
