/*
 * =====================================================================================
 * 
 *       Filename:  MainWindow.h
 * 
 *    Description:  程序的主窗口
 * 
 *        Version:  1.0
 *        Created:  2007年11月25日 12时56分29秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */


#ifndef _MAINWINDOW_HH
#define _MAINWINDOW_HH

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <iostream>
#include "gmplayer.h"
#include "channel.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"
#include "live_player.h"
#include "StreamMenu.h"

#define main_ui	    DATA_DIR"/gmlive.xml"
typedef Glib::RefPtr < Gnome::Glade::Xml > GlademmXML;

class MainWindow : public Gtk::Window {
	public:
		MainWindow();
		~MainWindow();
		void init();
		void play(LivePlayer* lp);
		void record(LivePlayer* lp);
		RecentChannel& getRecentChannel()
		{ return dynamic_cast<RecentChannel&>(*recentChannel);}
		BookMarkChannel& getBookMarkChannel()
	       	{ return dynamic_cast<BookMarkChannel&>(*bookMarkChannel); }
		StreamMenu& getMenu() { return streamMenu;}
		GMplayer& get_gmp() const { return *gmp; }
	protected:
		Channel* get_cur_channel();
	public:
		/** 菜单里的回调函数*/
		void on_menu_play_activate();
		void on_menu_record_activate();
		void on_menu_add_activate();
		void on_menu_refresh_activate();
		void on_menu_expand_activate();
		void on_menu_collapse_activate();
	private:
		void on_fullscreen();
		void on_stop();
		void on_play();
		void on_record();
		bool on_gmplayer_out(const Glib::IOCondition& condition);
		void on_live_player_status(int percentage);
		void on_gmplayer_start();
		void on_gmplayer_stop();
		void on_toggle_player();
		void show_msg(const Glib::ustring& msg,unsigned int id=0);

	private:
		GlademmXML ui_xml;
		Gtk::CheckButton* checkplayer;
		GMplayer* gmp;
		LivePlayer* live_player;
		Channel* 	recentChannel;
		Channel* 	bookMarkChannel;
		Gtk::Statusbar* statusbar;
		Gtk::Notebook* picture;
		Gtk::Notebook* listNotebook;
		StreamMenu streamMenu;
		//GKeyFile* keyfile;
		//Setting* conf;
};

#endif // _MAINWINDOW_HH 

