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
#include "mmschannel.h"
#include "nslivechannel.h"
#include "sopcastchannel.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"
#include "live_player.h"

#include "MainWindow.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <cassert>
using namespace std;
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

Glib::ustring ui_info =
"<ui>"
"	<menubar name='MenuBar'>"
"		<menu action='FileMenu'>"
"			<menuitem action='FilePlay'/>"
"			<menuitem action='FileStop'/>"
"			<separator/>"
"			<menuitem action='FileQuit'/>"
"        	</menu>"
"		<menu action='ViewMenu'>"
"			<menuitem action='ViewShowChannel'/>"
"			<menuitem action='ViewPreferences'/>"
"		</menu>"
"		<menu action='HelpMenu'>"
"			<menuitem action='HelpAbout'/>"
"		</menu>"
"	</menubar>"
"</ui>";


Gtk::Widget* MainWindow::create_main_menu()
{
	if (!action_group)
		action_group = Gtk::ActionGroup::create();
	//File menu:
	action_group->add(Gtk::Action::create("FileMenu", "文件(_F)"));
	action_group->add(Gtk::Action::create("FilePlay", Gtk::Stock::MEDIA_PLAY),
			sigc::mem_fun(*this, &MainWindow::on_menu_file_play));
	action_group->add(Gtk::Action::create("FileStop", Gtk::Stock::MEDIA_STOP),
			sigc::mem_fun(*this, &MainWindow::on_menu_file_stop));
	action_group->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
			sigc::mem_fun(*this, &MainWindow::on_menu_file_quit));

	//View menu:
	action_group->add(Gtk::Action::create("ViewMenu", "查看(_V)"));
	action_group->add(Gtk::ToggleAction::create("ViewShowChannel", 
				"显示频道列表(_S)", "隐藏或显示频道列表", true),
			sigc::mem_fun(*this, &MainWindow::on_menu_view_show_channel));
	action_group->add(Gtk::Action::create("ViewPreferences", Gtk::Stock::PREFERENCES),
			sigc::mem_fun(*this, &MainWindow::on_menu_view_preferences));

	//Help menu:
	action_group->add(Gtk::Action::create("HelpMenu", "帮助(_H)"));
	action_group->add(Gtk::Action::create("HelpAbout", Gtk::Stock::HELP),
			sigc::mem_fun(*this, &MainWindow::on_menu_help_about));

	if (!ui_manager)
		ui_manager = Gtk::UIManager::create();

	ui_manager->insert_action_group(action_group);
	add_accel_group(ui_manager->get_accel_group());
	ui_manager->add_ui_from_string(ui_info);

	return ui_manager->get_widget("/MenuBar");
}

void MainWindow::on_menu_file_play()
{
	cout << "on_menu_file_play" << endl;
	//gmp->start("mms://61.139.37.135/star");
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->play_selection();
	else
		cout << "Error" << endl;
	
}

void MainWindow::on_menu_file_stop()
{
	cout << "on_menu_stop" << endl;
	gmp->stop();
}

void MainWindow::on_menu_file_quit()
{
	cout << "on_menu_file_quit" << endl;
}

void MainWindow::on_menu_view_show_channel()
{
	cout << "on_menu_view_show_channel" << endl;

	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> show = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewShowChannel"));

	if (show->get_active())
		channels->show();
	else
		channels->hide();
	this->resize(1, 1);
}

void MainWindow::on_menu_view_preferences()
{
	cout << "on_menu_view_preferences" << endl;
}

void MainWindow::on_menu_help_about()
{
	cout << "on_menu_help_about" << endl;
}

void MainWindow::on_gmplayer_start()
{
	reorder_widget(true);
}

void MainWindow::on_gmplayer_stop()
{
	show_msg("ready...");
	reorder_widget(false);
	live_player = NULL;
}

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

void MainWindow::on_live_player_out(int percentage)
{
	char buf[256];
	sprintf(buf, "Connect...%%%d", percentage);
	show_msg(buf, sizeof (buf));
}

MainWindow::MainWindow():
	live_player(NULL)
{
	ui_xml = Gnome::Glade::Xml::create(main_ui, "mainFrame");
	if (!ui_xml) 
		exit(127);
	Gtk::VBox* main_frame = 
		dynamic_cast < Gtk::VBox* >
		(ui_xml->get_widget("mainFrame"));

	statusbar = dynamic_cast<Gtk::Statusbar*>
		(ui_xml->get_widget("statusbar"));

	play_frame = dynamic_cast<Gtk::Box*>
		(ui_xml->get_widget("playFrame"));

	channels = dynamic_cast<Gtk::Notebook*>
		(ui_xml->get_widget("channels"));

	Channel* channel = Gtk::manage(new class MMSChannel(this));
	Gtk::ScrolledWindow* swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("mmsChannelWnd"));
	swnd->add(*channel);

	channel = Gtk::manage(new class SopcastChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("sopcastChannelWnd"));
	swnd->add(*channel);

	channel = Gtk::manage(new class NSLiveChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("nsliveChannelWnd"));
	swnd->add(*channel);

	recent_channel = Gtk::manage(new class RecentChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("recentChannelWnd"));
	swnd->add(*recent_channel);

	bookmark_channel = Gtk::manage(new class BookMarkChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("bookmarkChannelWnd"));
	swnd->add(*bookmark_channel);


	backgroup = new Gtk::Image(
			Gdk::Pixbuf::create_from_file (
				"../data/gmlive_play.png"));

	gmp = new GMplayer(sigc::mem_fun(*this, &MainWindow::on_gmplayer_out));	
	gmp->signal_start_play().connect(sigc::mem_fun(*this, &MainWindow::on_gmplayer_start));
	gmp->signal_stop_play().connect(sigc::mem_fun(*this, &MainWindow::on_gmplayer_stop));

	menubar = create_main_menu();

	play_frame->pack_start(*backgroup, true, true);

	main_frame->pack_start(*menubar, false, false);

	this->add(*main_frame);

	Glib::RefPtr<Gdk::Pixbuf> pix = Gdk::Pixbuf::create_from_file("../data/gmlive.png");
	this->set_icon(pix);

	this->show_all();
	//channels->hide();
	this->resize(1,1);
}

MainWindow::~MainWindow()
{
	delete backgroup;
	delete gmp;
}



void MainWindow::show_msg(const Glib::ustring& msg, unsigned int id)
{
	statusbar->pop(id);
	statusbar->push(msg, id);
}

void MainWindow::reorder_widget(bool is_running)
{
	if (is_running){
		static int width = backgroup->get_width();
		static int height = backgroup->get_height();
		printf("%d,%d\n", width, height);
		play_frame->remove(*backgroup);
		play_frame->pack_start(*gmp, true, true);
		gmp->show_all();
		gmp->set_size_request(width, height);
	}
	else {
		play_frame->remove(*gmp);
		play_frame->pack_start(*backgroup, true, true);
		backgroup->show_all();
	}
	this->resize(1,1);
}

Channel* MainWindow::get_cur_select_channel()
{
	int npage = channels->get_current_page();
	Gtk::Container* page = 
		dynamic_cast<Gtk::Container*>(channels->get_nth_page(npage));
	if (!page)
		return NULL;
	return dynamic_cast< Channel* >(*(page->get_children().begin()));
}

void MainWindow::set_live_player(LivePlayer* lp)
{
	if (lp != live_player) {
		delete live_player;
		live_player = lp;
		lp->signal_status().connect(sigc::mem_fun(
					*this, &MainWindow::on_live_player_out));
	}
}

