
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
	char buf2[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/sopcast.lst.tmp",homedir);
	snprintf(buf2, 512,"%s/.gmlive/sopcast.lst",homedir);

	if (rename(buf, buf2))
		return;

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
		//close(STDOUT_FILENO);
		//close(STDERR_FILENO);
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/sopcast.lst.tmp",homedir);

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
					    

void SopcastChannel::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/sopcast.lst",homedir);
	
#if 0
	m_liststore->clear();
	xmlDoc* doc = xmlReadFile(buf, NULL, 0);
	if (!doc) {
		std::cout <<"file error: " << buf << std::endl;
		goto backup_handle;
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	if (!root_element) {
		xmlCleanupParser();
		std::cout << "file is empty\n";
		goto backup_handle;
	}

	parse_channels(root_element);
	
	xmlCleanupParser();
	return;
#endif

	//如果读取默认列表正确则退出
	if(read_channels(buf))
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
					out = read_channels(filename);
					if(!out) //本地的频道列表打开错误处理
					{
					Gtk::MessageDialog warnDialog(_("File error"),
							false);
					warnDialog.set_secondary_text(_("the local channles list file has wrong formats"));
					warnDialog.run();
					}
					else
					{
						//这里处理把选择的文件替换成~/.gmlive/sopcast.lst

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
bool SopcastChannel::read_channels(const std::string& filename)
{
	m_liststore->clear();
	xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);
	if (!doc) {
		std::cout <<"file error: " << filename << std::endl;
		return false;
	}

	xmlNode* root_element = xmlDocGetRootElement(doc);
	if (!root_element) {
		xmlCleanupParser();
		std::cout << "file is empty\n";
		return false;
	}

	parse_channels(root_element);
	
	xmlCleanupParser();
	return true;
}


