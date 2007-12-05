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
#include "nslivechannel.h"
#include "mmschannel.h"
#include "sopcastchannel.h"
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
		/** 通用型播放函数，各种流播放调用入口
		 * @param stream	feed给mpalyer的流,mms流等
		 * @param TypeChannel   用于标识传入的流的类型
		 */
		/** i don't know how to decribe it in english
		 * @param stream the stream feed to mplayer. example mms://
		 * @param TypeChannel indentify the stream type.
		 */
		void play(const std::string& stream,TypeChannel type);
		RecentChannel& getRecentChannel(){ return *recentChannel;}
		BookMarkChannel& getBookMarkChannel() { return *bookMarkChannel; }
		StreamMenu& getMenu() { return streamMenu;}
	public:
		/** 菜单里的回调函数*/
		void on_menu_play_activate();
		void on_menu_record_activate();
		void on_menu_add_activate();
		void on_menu_refresh_activate();
	private:
		void on_fullscreen();
		void on_stop();
		void on_play();
		void on_record();
		bool on_gmplayer_out(const Glib::IOCondition& condition);
		void on_gmplayer_start();
		void on_gmplayer_stop();
		void show_msg(const Glib::ustring& msg,unsigned int id=0);

	private:
		GlademmXML ui_xml;
		GMplayer* gmp;
		LivePlayer* live_player;
		NSLiveChannel* 	nsliveChannel;
		MMSChannel*	mmsChannel;
		SopcastChannel* sopcastChannel;
		RecentChannel* 	recentChannel;
		BookMarkChannel* bookMarkChannel;
		Gtk::Statusbar* statusbar;
		Gtk::Notebook* picture;
		Gtk::Notebook* listNotebook;
		StreamMenu streamMenu;
};

#endif // _MAINWINDOW_HH 

