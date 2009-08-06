/*
 * =====================================================================================
 *
 *       Filename:  pplivechannel.h
 *
 *    Description:  pplive的列表支持
 *
 *        Version:  1.0
 *        Created:  2009年08月06日 14时27分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#ifndef	PPLIVECHANNEL_FILE_HEADER_INC
#define PPLIVECHANNEL_FILE_HEADER_INC

#include "channel.h"
#include <functional>
class GMplayer;

class PpliveChannel : public Channel {
	public:
		PpliveChannel(MainWindow* parent_);
		void init();

		bool read_channels(const char* filename);
		void addLine(int users, const Glib::ustring& name,
			   	const std::string&sream, const Glib::ustring& groupname);
		void refresh_list();

		typedef sigc::signal<void> type_signal_stop;
		type_signal_stop signal_stop_refresh()
		{ return signal_stop_refresh_; }

		typedef sigc::signal<void> type_signal_start;
		type_signal_start signal_start_refresh()
		{ return signal_start_refresh_; }
		
	protected:
		LivePlayer* get_player(const std::string& stream, TypeChannel page);
		void wait_wget_exit(GPid pid, int);

	private:
		Gtk::TreeModel::iterator get_group_iter(const char* name);
		void parse_channel(xmlNode* a_node);
		void parse_channels(xmlNode* a_node);
		
		type_signal_stop	signal_stop_refresh_;
		type_signal_start 	signal_start_refresh_;

		int wget_pid;
		bool refresh;
};

#endif // PPLIVECHANNEL_FILE_HEADER_INC
