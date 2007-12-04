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
//#include <functional>

/*
#define NSLIVESTREAM "http://127.0.0.1:9000"
enum TypeChannel{
	NSLIVE_CHANNEL,
	MMS_CHANNEL,
	SPOCAST_CHANNEL,
	GROUP_CHANNEL,
	NONE
};
*/

class Channel:public Gtk::TreeView
{
	public:
		Channel();
		virtual void init()=0;
		virtual ~Channel();
		Gtk::TreeModel::iterator getListIter(Gtk::TreeModel::
				Children children, const std::string& groupname);
		
		virtual void  addLine(const int id,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname)=0;
		Gtk::TreeModel::iterator addGroup(const Glib::ustring& group);

	public:
		class ChannelColumns:public Gtk::TreeModel::ColumnRecord{
			public:
				ChannelColumns()
				{
					add(id);
					add(name);
					add(stream);
					add(freq);
					add(type);
				}
				Gtk::TreeModelColumn<int> id;
				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<std::string> stream;
				Gtk::TreeModelColumn<int> freq;
				Gtk::TreeModelColumn<TypeChannel> type;
		};

		ChannelColumns columns;
		//Glib::RefPtr< Gtk::ListStore> m_liststore;
		Glib::RefPtr< Gtk::TreeStore> m_liststore;
	protected:
		//bool on_button_press_event(GdkEventButton *);
/*
	private:
		struct CompareChannel:public binary_function < Gtk::TreeModel::Row,
		const Glib::ustring,bool >{
			explicit CompareChannel(const ChannelColumns& 
					column_):column(column_) {
			} bool operator () (const Gtk::TreeRow& lhs,
					const Glib::ustring& var) const {
				return lhs[column.name] == var;
			} const ChannelColumns& column;
		};
		*/

};


#endif   /* ----- #ifndef CHANNEL_FILE_HEADER_INC  ----- */

