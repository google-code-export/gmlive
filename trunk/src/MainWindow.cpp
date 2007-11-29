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

MainWindow::MainWindow()
{
	ui_xml = Gnome::Glade::Xml::create(main_ui, "mainFrame");
	Gtk::VBox* vbox = 
		dynamic_cast < Gtk::VBox* >
		(ui_xml->get_widget("mainFrame"));

	Gtk::HBox* hbox = dynamic_cast < Gtk::HBox* >
		(ui_xml->get_widget("hFrame"));
	
	Gtk::Button* bt_fullscreen=dynamic_cast <Gtk::Button* >
		(ui_xml->get_widget("bt_fullscreen"));
	gmp = new GMplayer;	
	if (hbox)
		hbox->pack_end(*gmp, true, true);

	bt_fullscreen->signal_clicked().
		connect(sigc::mem_fun(*this, &MainWindow::on_fullscreen));
	this->add(*vbox);
	this->show_all();
	gmp->start("a.avi");
}

void MainWindow::on_fullscreen()
{
	gmp->full_screen();
}
