/*
 * =====================================================================================
 *
 *       Filename:  MainWindow.cpp
 *
 *    Description:   程序的主窗 
 *
 *        Version:  1.0
 *        Created:  2007年11月25日 13时00分30秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#include "MainWindow.h"
#include "gmmplayer.h"

MainWindow::MainWindow()
{
	ui_xml = Gnome::Glade::Xml::create(main_ui, "mainFrame");
	Gtk::VBox* vbox = 
		dynamic_cast < Gtk::VBox* >
		(ui_xml->get_widget("mainFrame"));

	Gtk::HBox* hbox = dynamic_cast < Gtk::HBox* >
		(ui_xml->get_widget("hFrame"));
	
	GMMplayer* gmp = new GMMplayer;	
	//if (hbox)
		hbox->pack_end(*gmp, true, true);

	this->add(*vbox);
	this->show_all();
	//gmp->start("a.avi");
}

