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

#define main_ui	    DATA_DIR"/gmlive.xml"
typedef Glib::RefPtr < Gnome::Glade::Xml > GlademmXML;

class MainWindow : public Gtk::Window {
	public:
		MainWindow();
		void on_fullscreen();
		void on_stop();
		void on_play();
		void on_record();
		void nslive_play(int channel_num);
		void mms_play(const std::string& stream);
		void showMsg(const std::string& msg,unsigned int id=0);

	private:
		GlademmXML ui_xml;
		GMplayer* gmp;
		NSLiveChannel* nsliveChannel;
		MMSChannel*	mmsChannel;
		Gtk::Statusbar* statusbar;
};

#endif // _MAINWINDOW_HH 

