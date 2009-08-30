/*
 * =====================================================================================
 *
 *       Filename:  ppschannel.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月02日 09时16分15秒 CST
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
#include <sys/types.h>
#include <sys/wait.h>
#include "gmlive.h"
#include "ppschannel.h"
#include "MainWindow.h"
#include "ppsPlayer.h"

PPSChannel::PPSChannel(MainWindow* parent_):
	Channel(parent_)
	,wget_pid(-1)
	,refresh(false)
{
	refresh_list();
}

LivePlayer* PPSChannel::get_player(const std::string& stream,TypeChannel page)
{
	return PPSPlayer::create(stream);
}

bool PPSChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/pps.lst",homedir);

	m_liststore->clear(); //先清空列表
	read_channels(buf);


	return true;
}


void PPSChannel::wait_wget_film_url_exit(GPid pid, int)
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/pps_url.lst",homedir);

	xmlDoc* doc = xmlReadFile(buf, NULL, 0);
	xmlNode* root_element = xmlDocGetRootElement(doc);
	ScopeGuard a = MakeGuard(&xmlCleanupParser);

	if (!root_element) {
		std::cerr << "pps file is emtpy: " << buf << std::endl;
		return ;
	}

	xmlNode* a_node = root_element->children;

	for (; a_node; a_node = a_node->next) {
		if (xmlStrcmp(a_node->name, (const xmlChar*)"Film") == 0) {
			Gtk::TreeModel::iterator iter = m_liststore->append(cur_refresh_iter2->children());

			xmlNode* cur_node = a_node->children;
			cur_node = cur_node->next;
			xmlChar* p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.name] = (const char*)p;
			std::cout << p << std::endl;
			xmlFree(p);

			//p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			//(*iter)[columns.name] = (const char*)p;
			//xmlFree(p);
		}

	}
	
	if ((*cur_refresh_iter2)[columns.pps_num] > cur_refresh_iter2->children().size())
		cur_refresh_page++;
	else {
		if (!step_iterator())
			return;
		else
			cur_refresh_page = 1;
	}

	refresh_film_url();
}

bool PPSChannel::step_iterator()
{
	if (cur_refresh_iter == cur_refresh_iter2) {
		++cur_refresh_iter;

		if (cur_refresh_iter == m_liststore->children().end())
			return false;

		if ((*cur_refresh_iter)[columns.pps_type] == 1) {
			cur_refresh_iter2 = cur_refresh_iter;
		} else  {
			cur_refresh_iter2 = cur_refresh_iter->children().begin();
		}
		return true;
	}

	++cur_refresh_iter2;
	if (cur_refresh_iter2 == cur_refresh_iter->children().end()) {
		++cur_refresh_iter;

		if (cur_refresh_iter == m_liststore->children().end())
			return false;

		if ((*cur_refresh_iter)[columns.pps_type] == 1) {
			cur_refresh_iter2 = cur_refresh_iter;
		} else  {
			cur_refresh_iter2 = cur_refresh_iter->children().begin();
		}
		return true;
	}
	return true;
}

void PPSChannel::refresh_film_url()
{

	if (cur_refresh_iter == m_liststore->children().end() || cur_refresh_iter2 == cur_refresh_iter->children().end())
		return;


	int pid = fork();
	if (pid == -1)
		return;
	if (pid == 0) {
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/pps_url.lst",homedir);

		char buf2[512];
		int classid = (*cur_refresh_iter)[columns.pps_id];
		int subclassid = (*cur_refresh_iter2)[columns.pps_id];
		if ((*cur_refresh_iter)[columns.pps_type] == 1)
			snprintf(buf2, 512, "http://playlist.pps.tv/channelsfortv.php?class=%d&page=%d", classid, cur_refresh_page);
		else
			snprintf(buf2, 512, "http://playlist.pps.tv/channelsfortv.php?class=%d&subclass=%d&page=%d", classid, subclassid, cur_refresh_page);

		const char* argv[6];
		argv[0] = "wget";
		argv[1] = buf2;
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

		execvp("wget", (char* const *)argv);
		perror("wget execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &PPSChannel::wait_wget_film_url_exit), pid);

}

void PPSChannel::wait_wget_channel_exit(GPid pid, int)
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/pps_channel.lst",homedir);

	xmlDoc* doc = xmlReadFile(buf, NULL, 0);
	xmlNode* root_element = xmlDocGetRootElement(doc);
	ScopeGuard a = MakeGuard(&xmlCleanupParser);

	if (!root_element) {
		std::cerr << "pps file is emtpy: " << buf << std::endl;
		return ;
	}

	xmlNode* a_node = root_element->children;

	for (; a_node; a_node = a_node->next) {
		if (xmlStrcmp(a_node->name, (const xmlChar*)"SubClass") == 0) {
			Gtk::TreeModel::iterator iter = m_liststore->append(cur_refresh_iter->children());

			xmlNode* cur_node = a_node->children;
			cur_node = cur_node->next;

			xmlChar* p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.pps_id] = atoi((const char*)p);
			xmlFree(p);

			p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.name] = (const char*)p;
			xmlFree(p);
		}

	}

	++cur_refresh_iter;
	refresh_channel();
}

void PPSChannel::refresh_channel()
{
	while (cur_refresh_iter != m_liststore->children().end()) {
		if ((*cur_refresh_iter)[columns.pps_type] == 0)
			break;
		++cur_refresh_iter;
	}
	if (cur_refresh_iter == m_liststore->children().end()) {
		cur_refresh_iter = m_liststore->children().begin();

		if ((*cur_refresh_iter)[columns.pps_type] == 1) {
			cur_refresh_iter2 = cur_refresh_iter;
		} else  {
			cur_refresh_iter2 = cur_refresh_iter->children().begin();
		}
		cur_refresh_page = 1;

		refresh_film_url();
		return;
	}

	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/pps_channel.lst",homedir);

		char buf2[512];
		int id = (*cur_refresh_iter)[columns.pps_id];
		snprintf(buf2, 512, "http://playlist.pps.tv/subclassfortv.php?class=%d", id);

		const char* argv[6];
		argv[0] = "wget";
		argv[1] = buf2;
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

		execvp("wget", (char* const *)argv);
		perror("wget execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &PPSChannel::wait_wget_channel_exit), pid);

}

void PPSChannel::parse_channels(xmlNode* a_node)
{
	for (xmlNode* node = a_node->children;
			node; node = node->next) {
		if ((node->type == XML_ELEMENT_NODE ) &&
				(!xmlStrcmp(node->name, (const xmlChar*)"Class"))) {

			xmlNode* cur_node = node->children;
			if (!cur_node) 
				return;

			Gtk::TreeModel::iterator iter = m_liststore->append();

			// ID
			cur_node = cur_node->next;
			xmlChar* p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.pps_id] = atoi((const char*)p);
			xmlFree(p);

			// Title
			cur_node = cur_node->next;
			cur_node = cur_node->next;
			p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.name] = (const char*)p;
			xmlFree(p);

			// Type
			cur_node = cur_node->next;
			cur_node = cur_node->next;
			p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.pps_type] = atoi((const char*)p);
			xmlFree(p);

			// ContentNum
			cur_node = cur_node->next;
			cur_node = cur_node->next;
			p =  xmlNodeListGetString(cur_node->doc, cur_node->xmlChildrenNode, 1);
			(*iter)[columns.pps_num] = atoi((const char*)p);
			xmlFree(p);
			(*iter)[columns.type]= GROUP_CHANNEL;

		}
	}

	cur_refresh_iter = m_liststore->children().begin();
	refresh_channel();
}


bool PPSChannel::read_channels(const char* filename)
{
	xmlDoc* doc = xmlReadFile(filename, NULL, 0);
	if (!doc) {
		std::cerr << filename << " open fail." << std::endl;
		return false;
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	ScopeGuard a = MakeGuard(&xmlCleanupParser);

	if (!root_element) {
		std::cerr << "pps file is emtpy: " << filename << std::endl;
		return false;
	}

	parse_channels(root_element);
	return true;
}


void PPSChannel::wait_wget_exit(GPid pid, int)
{
	if (wget_pid != -1) {
		waitpid(wget_pid, NULL, 0);
		wget_pid = -1;

		refresh = false;
	}

	char buf[512];
	char buf2[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/pps.lst.tmp",homedir);
	snprintf(buf2, 512,"%s/.gmlive/pps.lst",homedir);

	if (rename(buf, buf2))
		return;

	init();
	signal_stop_refresh_.emit();
}


void PPSChannel::refresh_list()
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
		snprintf(buf, 512,"%s/.gmlive/pps.lst.tmp",homedir);

		const char* argv[6];
		argv[0] = "wget";
		argv[1] = "http://playlist.pps.tv/classfortv.php";
		//argv[1] = GMConf["pps_channel_url"].c_str();
		argv[2] = "-O";
		argv[3] = buf;
		argv[4] = "-q";
		argv[5] = NULL;

		execvp("wget", (char* const *)argv);
		perror("wget execvp:");
		exit(127);
	} 
	Glib::signal_child_watch().connect
		(sigc::mem_fun(*this, &PPSChannel::wait_wget_exit), pid);

}
