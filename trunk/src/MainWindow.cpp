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
k* =====================================================================================
 */

#include "MainWindow.h"
#include "mms_live_player.h"
#include "ns_live_player.h"

Glib::ustring get_print_string(const char* buf, int len)
{
	if (len < 4)
		return Glib::ustring();
	len -= 3;
	char new_buf[len];
	char* pnew = new_buf;
	const char* pend = buf + len;
	for(;buf < pend; ++buf) {
		if (iscntrl(*buf))
			continue;
		*pnew++ = *buf;
	}
	*pnew = 0;
	return Glib::ustring(new_buf, pnew);
}


MainWindow::MainWindow():
	live_player(NULL),
	streamMenu(*this)
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
	statusbar = dynamic_cast<Gtk::Statusbar*>
		(ui_xml->get_widget("statusbar"));
	picture = dynamic_cast<Gtk::Notebook*>
		(ui_xml->get_widget("notebook_picture"));
	listNotebook = dynamic_cast<Gtk::Notebook*>
		(ui_xml->get_widget("listnotebook"));

	gmp = new GMplayer(sigc::mem_fun(*this, &MainWindow::on_gmplayer_out));	
	gmp->signal_start_play().connect(sigc::mem_fun(*this, &MainWindow::on_gmplayer_start));
	gmp->signal_stop_play().connect(sigc::mem_fun(*this, &MainWindow::on_gmplayer_stop));

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
	recentChannel = Gtk::manage(new class RecentChannel(this));
	Gtk::ScrolledWindow* scrolledwin_recent= dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("scrolledwin_recent"));
	scrolledwin_recent->add(*recentChannel);

	bookMarkChannel = Gtk::manage(new class BookMarkChannel(this));
	Gtk::ScrolledWindow* scrolledwin_bookmark = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("scrolledwin_bookmark"));
	scrolledwin_bookmark->add(*bookMarkChannel);

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
	recentChannel->init();

	this->show_all();
	
}

MainWindow::~MainWindow()
{
	delete live_player;
}

void MainWindow::show_msg(const Glib::ustring& msg, unsigned int id)
{
	statusbar->pop(id);
	statusbar->push(msg, id);
}
void MainWindow::on_fullscreen()
{
	gmp->full_screen();
}

void MainWindow::on_stop()
{
	if (live_player)
		live_player->stop();
	delete live_player;
	live_player = NULL;
}

void MainWindow::on_play()
{
}

void MainWindow::on_record()
{
}

void MainWindow::play(int channel_num,const std::string& stream,TypeChannel type)
{
	on_stop();
	if(NSLIVE_CHANNEL == type)
		live_player = new NsLivePlayer(*gmp, channel_num);
	else if(MMS_CHANNEL == type)
		live_player = new MmsLivePlayer(*gmp, stream);
	else if(SOPCAST_CHANNEL ==type)
		return;//live_player = new SopcastLivePlayer(*gmp, stream);
	else
		return;
	live_player->play();
}
/*
void MainWindow::nslive_play(int channel_num)
{
	on_stop();
	live_player = new NsLivePlayer(*gmp, channel_num);
	live_player->play();
}

void MainWindow::mms_play(const std::string& stream)
{
	on_stop();
	live_player = new MmsLivePlayer(*gmp, stream);
	live_player->play();
}
*/


bool MainWindow::on_gmplayer_out(const Glib::IOCondition& condition)
{
	char buf[256];
	while (int len = gmp->get_mplayer_log(buf, 256)) {
		if (len < 256) {
			buf[len] = 0;
			show_msg(get_print_string(buf, len));
			break;
		}
	}
	return true;
}

void MainWindow::on_gmplayer_start()
{

	picture->set_current_page(PAGE_MPLAYER);
}

void MainWindow::on_gmplayer_stop()
{
	on_stop();
	show_msg("ready...");
	picture->set_current_page(PAGE_PICTURE);
}




void MainWindow::on_menu_play_activate()
{
	int page = listNotebook->get_current_page();

	Glib::RefPtr < Gtk::TreeSelection > selection ;

	if(page == NSLIVE_CHANNEL)
		nsliveChannel->play_selection();
	else if(MMS_CHANNEL == page)
		mmsChannel->play_selection();
	else if(SOPCAST_CHANNEL == page)
		;
}
void MainWindow::on_menu_record_activate()
{
	int page = listNotebook->get_current_page();

	Glib::RefPtr < Gtk::TreeSelection > selection ;

	if(page == NSLIVE_CHANNEL)
		nsliveChannel->record_selection();
	else if(MMS_CHANNEL == page)
		mmsChannel->record_selection();
	else if(SOPCAST_CHANNEL == page)
		;
}
void MainWindow::on_menu_add_activate()
{
	int page = listNotebook->get_current_page();

	Glib::RefPtr < Gtk::TreeSelection > selection ;

	if(page == NSLIVE_CHANNEL)
		nsliveChannel->store_selection();
	else if(MMS_CHANNEL == page)
		mmsChannel->store_selection();
	else if(SOPCAST_CHANNEL == page)
		;
}
void MainWindow::on_menu_refresh_activate()
{
}
