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
#include "gmlive.h"
#include <functional>

class MainWindow;
class LivePlayer;
class GMplayer;
class Channel:public Gtk::TreeView
{
	public:
		Channel(MainWindow* parent_);
		virtual ~Channel();
		void play_selection();
		void record_selection();
		void store_selection();
		virtual void refresh_list(){};

		Gtk::TreeModel::iterator getListIter(Gtk::TreeModel::
				Children children, const std::string& groupname);
		
		Gtk::TreeModel::iterator addGroup(const Glib::ustring& group);

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
				}
				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<std::string> stream;
				Gtk::TreeModelColumn<int> freq;
				Gtk::TreeModelColumn<int> users;
				Gtk::TreeModelColumn<TypeChannel> type;
		};

		ChannelColumns columns;
		Glib::RefPtr< Gtk::TreeStore> m_liststore;

	protected:
		virtual LivePlayer* get_player(
				const std::string& stream,TypeChannel page = NONE ) = 0;

		bool on_button_press_event(GdkEventButton *);
		void play_selection_iter(Gtk::TreeModel::iterator& iter);
		MainWindow* parent;
		LivePlayer* live_player;
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

