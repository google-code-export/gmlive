/*
 * =====================================================================================
 * 
 *       Filename:  channel.h
 * 
 *    Description:  频道列表
 * 
 *        Version:  1.0
 *        Created:  2007年12月01日 19时24分41秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  CHANNEL_FILE_HEADER_INC
#define  CHANNEL_FILE_HEADER_INC

#include <gtkmm.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/tooltips.h>
#include "gmlive.h"
#include <functional>

class MainWindow;
class LivePlayer;
class GMplayer;
class ChannelsTooltips;
class Channel:public Gtk::TreeView
{
	public:
		Channel(MainWindow* parent_);
		virtual ~Channel();
		void play_selection();
		void record_selection();
		void store_selection();
		std::string get_stream();
		void search_channel(const Glib::ustring& name);
		virtual void refresh_list(){};

		Gtk::TreeModel::iterator getListIter(Gtk::TreeModel::
				Children children, const std::string& groupname);
		
		Gtk::TreeModel::iterator addGroup(const Glib::ustring& group);

		bool on_tooltip_show(int x, int y, bool key_mode, const Glib::RefPtr<Gtk::Tooltip>& tooltip);
	public:
		class ChannelColumns:public Gtk::TreeModel::ColumnRecord{
			public:
				ChannelColumns()
				{
					add(name);
					add(stream);
					add(freq);
					add(type);
					add(users);
					add(searched);
				}
				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<std::string> stream;
				Gtk::TreeModelColumn<int> freq;
				Gtk::TreeModelColumn<int> users;
				Gtk::TreeModelColumn<TypeChannel> type;
				Gtk::TreeModelColumn<bool> searched;
		};

		ChannelColumns columns;
		Glib::RefPtr< Gtk::TreeStore> m_liststore;

	protected:
		virtual LivePlayer* get_player(
				const std::string& stream,
				TypeChannel page = NONE ) = 0;

		bool on_button_press_event(GdkEventButton *);
		bool on_visible_func(const Gtk::TreeModel::iterator& iter);
		void play_selection_iter(Gtk::TreeModel::iterator& iter);

		Glib::ustring search_channel_name;
		sigc::connection tipTimeout;
		ChannelsTooltips* tooltips;
		MainWindow* parent;
	protected:
		Glib::ustring user_select_list(const char* title);
		bool on_motion_event(GdkEventMotion* ev);
		bool on_leave_event(GdkEventCrossing * ev);
		bool tooltip_timeout(GdkEventMotion* ev);
	private:
		struct CompareChannel:public std::binary_function 
				      < Gtk::TreeModel::Row,
		const Glib::ustring,bool >{
			explicit CompareChannel(const ChannelColumns& 
					column_):column(column_) {} 
			bool operator () (const Gtk::TreeRow& lhs,
					const Glib::ustring& var) const 
			{
				return lhs[column.name] == var; 
			}
			const ChannelColumns& column;
		};

};


#endif   /* ----- #ifndef CHANNEL_FILE_HEADER_INC  ----- */

