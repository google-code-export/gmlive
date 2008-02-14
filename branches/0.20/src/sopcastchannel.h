/*
 * =====================================================================================
 * 
 *       Filename:  sopcastchannel.h
 * 
 *    Description:  sopcast的列表支持
 * 
 *        Version:  1.0
 *        Created:  2007年12月04日 20时11分23秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  SOPCASTCHANNEL_FILE_HEADER_INC
#define  SOPCASTCHANNEL_FILE_HEADER_INC
#include "channel.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

class GMplayer;
class SopcastChannel:public Channel
{
	public:
		SopcastChannel(MainWindow* parent_);
		void init();
		void  addLine(const int users,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname);
		void refresh_list();
		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop_refresh()
		{ return signal_stop_refresh_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start_refresh()
		{ return signal_start_refresh_; }	

	protected:
		LivePlayer* get_player(const std::string& stream,TypeChannel page);
		void wait_wget_exit(GPid pid, int);
	private:
		int parse_channel (Gtk::TreeModel::iterator& iter, xmlNode* a_node);

		void parse_channels(xmlNode* a_node);
		void parse_group(xmlNode* a_node);

		type_signal_stop  signal_stop_refresh_;
		type_signal_start signal_start_refresh_;

		int wget_pid;
		bool refresh;

};



#endif   /* ----- #ifndef SOPCASTCHANNEL_FILE_HEADER_INC  ----- */

