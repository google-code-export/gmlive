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

#define main_ui	    DATA_DIR"/gmmlive.xml"
typedef Glib::RefPtr < Gnome::Glade::Xml > GlademmXML;

class MainWindow : public Gtk::Window {
	public:
		MainWindow();

	private:
		GlademmXML ui_xml;
};

#endif // _MAINWINDOW_HH 
