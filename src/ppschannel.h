/*
 * =====================================================================================
 * 
 *       Filename:  ppschannel.h
 * 
 *    Description:  support ordinarily pps stream like pps://xxx
 * 
 *        Version:  1.0
 *        Created:  2007年12月02日 09时12分37秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  PPSCHANNEL_FILE_HEADER_INC
#define  PPSCHANNEL_FILE_HEADER_INC

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
#include "channel.h"

class GMplayer;
class PPSChannel:public Channel
{
	public:
		PPSChannel(MainWindow* parent_);
		~PPSChannel(){}
		bool init();
		void refresh_list();
		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop_refresh()
		{ return signal_stop_refresh_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start_refresh()
		{ return signal_start_refresh_; }	
	protected:
		LivePlayer* get_player(const std::string& stream,TypeChannel page);
	private:
		void parse_channels(xmlNode* a_node);
		bool read_channels(const char* filename);
		void refresh_channel();
		void refresh_film_url();

		void wait_wget_exit(GPid pid, int);
		void wait_wget_channel_exit(GPid pid, int);
		void wait_wget_film_url_exit(GPid pid, int);
		bool step_iterator();

		type_signal_stop  signal_stop_refresh_;
		type_signal_start signal_start_refresh_;

		Gtk::TreeModel::iterator cur_refresh_iter;
		Gtk::TreeModel::iterator cur_refresh_iter2;
		int cur_refresh_page;
		int wget_pid;
		bool refresh;

};

#endif   /* ----- #ifndef PPSCHANNEL_FILE_HEADER_INC  ----- */

