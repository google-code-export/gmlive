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
//#include "MainWindow.h"


class MainWindow;

class Channel:public Gtk::TreeView
{
	public:
		Channel(MainWindow* parent_);
		void init();
		~Channel();
		
		void addLine(const int id,const Glib::ustring& name);
		//Gtk::TreeModel::iterator getListIter(Gtk::TreeModel::Children children,
		//		const int id);

	private:
		class ChannelColumns:public Gtk::TreeModel::ColumnRecord{
			public:
				ChannelColumns()
				{
					add(id);
					add(name);
					add(freq);
				}
				Gtk::TreeModelColumn<int> id;
				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<int> freq;
		};

	private:
		MainWindow* parent;
		ChannelColumns columns;
		int number;
		Glib::RefPtr< Gtk::ListStore> m_liststore;
	protected:
		bool on_button_press_event(GdkEventButton *);

};


#endif   /* ----- #ifndef CHANNEL_FILE_HEADER_INC  ----- */

