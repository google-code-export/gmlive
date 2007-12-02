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
	Gtk::Button* bt_stop=dynamic_cast <Gtk::Button* >
		(ui_xml->get_widget("bt_stop"));
	Gtk::Button* bt_record=dynamic_cast<Gtk::Button* >
		(ui_xml->get_widget("bt_record"));
	Gtk::Button* bt_play=dynamic_cast<Gtk::Button*>
		(ui_xml->get_widget("bt_play"));
	Gtk::Statusbar* statusbar  = dynamic_cast<Gtk::Statusbar*>
		(ui_xml->get_widget("statusbar"));

	gmp = new GMplayer(this);	
	if (hbox)
		hbox->pack_end(*gmp, true, true);

	nsliveChannel = Gtk::manage(new class NSLiveChannel(this));
	Gtk::ScrolledWindow* scrolledwin_nslive = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("scrolledwin_nslive"));
	scrolledwin_nslive->add(*nsliveChannel);
	mmsChannel = Gtk::manage(new class MMSChannel(this));
	Gtk::ScrolledWindow* scrolledwin_mms = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("scrolledwin_mms"));
	scrolledwin_mms->add(*mmsChannel);


	bt_fullscreen->signal_clicked().
		connect(sigc::mem_fun(*this, &MainWindow::on_fullscreen));
	bt_stop->signal_clicked().
		connect(sigc::mem_fun(*this, &MainWindow::on_stop));
	bt_record->signal_clicked().
		connect(sigc::mem_fun(*this, &MainWindow::on_record));
	bt_play->signal_clicked().
		connect(sigc::mem_fun(*this, &MainWindow::on_play));
	this->add(*vbox);

	mmsChannel->init();
	nsliveChannel->init();

	this->show_all();
	//gmp->start("a.avi");
	
}

void MainWindow::showMsg(const std::string& msg, unsigned int id)
{
	statusbar->pop();
	statusbar->push(msg);
}
void MainWindow::on_fullscreen()
{
	gmp->full_screen();
}

void MainWindow::on_stop()
{
	gmp->stop();
}

void MainWindow::on_play()
{
}

void MainWindow::on_record()
{
}

void MainWindow::nslive_play(int channel_num)
{
	//启动nslive 的进程，然后再启动mplayer
	gmp->nslive_play();

}
void MainWindow::mms_play(const std::string& stream)
{
	gmp->start(stream);
}
