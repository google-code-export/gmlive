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

#define NSLIVESTREAM "http://127.0.0.1:9000"
enum TypeChannel{
	NSLIVE_CHANNEL,
	MMS_CHANNEL,
	SPOCAST_CHANNEL,
	NONE
};

class Channel:public Gtk::TreeView
{
	public:
		//Channel(MainWindow* parent_);
		Channel();
		//void init();
		virtual void init()=0;
		virtual ~Channel();
		
		virtual void  addLine(const int id,const Glib::ustring& name,const std::string& sream)=0;
		//void addLine(const int id,const Glib::ustring& name);
		//Gtk::TreeModel::iterator getListIter(Gtk::TreeModel::Children children,
		//		const int id);

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

		//MainWindow* parent;
		ChannelColumns columns;
		Glib::RefPtr< Gtk::ListStore> m_liststore;
	protected:
		//bool on_button_press_event(GdkEventButton *);

};

/*
class NSLiveChannel:public Channel
{
	public:
		NSLiveChannel(MainWindow* parent_);
		void init();
		void  addLine(const int id,const Glib::ustring& name,const std::string& sream);

	private:
		MainWindow* parent;
	protected:
		bool on_button_press_event(GdkEventButton *);

};
*/
#endif   /* ----- #ifndef CHANNEL_FILE_HEADER_INC  ----- */

