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

	char new_buf[len];
	char* pnew = new_buf;
	const char* pend = buf + len;
	for(;buf < pend; ++buf) {
		if (!iscntrl(*buf))
			*pnew++ = *buf;
		else
			printf("%o\n", *buf);
	}
	*pnew = 0;
	return Glib::ustring(new_buf, pnew);
}

bool get_video_resolution(const char* buf, int& w, int& h)
{
	// 输出行应该是这样的
	// VO: [xv] 800x336 => 800x336 Planar YV12 

	if ( (buf[0] == 'V') &&
			(buf[1] == 'O') &&
			(buf[2] == ':')) {


		w = -1; 
		h = -1;
		const char* pw = strstr(buf, "] ");
		if (NULL == ++pw) 
			return false;
		w = atoi(pw);

		const char* ph = strstr(pw, "x");
		if (NULL == ++ph)
			return false;
		h = atoi(ph);

		printf("w = %d, h = %d\n", w, h);
		return true;
	}
	return false;
}


Glib::ustring ui_info =
"<ui>"
"	<menubar name='MenuBar'>"
"		<menu action='FileMenu'>"
"			<menuitem action='FilePlay'/>"
"			<menuitem action='FilePause'/>"
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
	action_group->add(Gtk::Action::create("FilePause", Gtk::Stock::MEDIA_PAUSE),
			sigc::mem_fun(*this, &MainWindow::on_menu_file_pause));
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

void MainWindow::on_menu_file_pause()
{
	cout << "on_menu_pause" << endl;
	gmp->pause();
}

void MainWindow::on_menu_file_quit()
{
	cout << "on_menu_file_quit" << endl;
	gmp->stop();
	Gtk::Main::quit();
}

bool MainWindow::on_delete_event(GdkEventAny* event)
{
	gmp->stop();
	return Gtk::Window::on_delete_event(event);
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
	Gtk::Window* about=Gtk::manage(new Gtk::Window(Gtk::WINDOW_POPUP));
	Gtk::VBox* about_frame=dynamic_cast<Gtk::VBox*>
		(ui_xml->get_widget("about_frame"));
	about->add(*about_frame);
	about->show();
}

void MainWindow::on_gmplayer_start()
{
	reorder_widget(true);
}

void MainWindow::on_gmplayer_stop()
{
	show_msg("ready...");
	reorder_widget(false);
}



bool MainWindow::on_gmplayer_out(const Glib::IOCondition& condition)
{
	char buf[256];
	int len = gmp->get_mplayer_log(buf, 255);
	buf[len] = 0;
	const char* pend = buf + len;
	char* p1 = buf;
	char* p2 = buf;
	for(; p1 < pend;) {
		if (!iscntrl(*p1)) {
			p1++;
		}
		else {
			*p1 = 0;
			show_msg(Glib::ustring(p2, p1));
			if (-1 == gmp_width) {
				if ((p1 - p2) > 5) {
					int w,h;
					if (get_video_resolution(p2, w, h))
						set_gmp_size(w, h);
				}
			}

			for (p1++; p1 < pend; p1++) {
				if (!iscntrl(*p1))
					break;
			}
			p2 = p1;
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
	,gmp_width(-1)
	,gmp_height(-1)
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
				DATA_DIR"/gmlive_play.png"));

	gmp = new GMplayer(sigc::mem_fun(*this, &MainWindow::on_gmplayer_out));	
	gmp->signal_start_play().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_start));
	gmp->signal_stop_play().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_stop));

	menubar = create_main_menu();

	Gtk::HBox* menu_box=dynamic_cast<Gtk::HBox*>
			(ui_xml->get_widget("hbox_menu"));
	menu_box->pack_start(*menubar,true,true);
	//main_frame->pack_start(*menubar, false, false);
	play_frame->pack_start(*backgroup, true, true);


	this->add(*main_frame);

	Glib::RefPtr<Gdk::Pixbuf> pix = Gdk::Pixbuf::create_from_file(DATA_DIR"/gmlive.png");
	this->set_icon(pix);

	this->show_all();
	//channels->hide();
	this->resize(1,1);
}

MainWindow::~MainWindow()
{
	delete backgroup;
	delete live_player;
	delete gmp;
}



void MainWindow::show_msg(const Glib::ustring& msg, unsigned int id)
{
	statusbar->pop(id);
	statusbar->push(msg, id);
}

void MainWindow::set_gmp_size(int w, int h)
{
	gmp_width = w;
	gmp_height = h;
	if (w != -1) {
		gmp->set_size_request(w, h);
		this->resize(1, 1);
	}
}

void MainWindow::reorder_widget(bool is_running)
{
	if (is_running){
		static int width = backgroup->get_width();
		static int height = backgroup->get_height();
		play_frame->remove(*backgroup);
		play_frame->pack_start(*gmp, true, true);
		gmp->show_all();
		if (gmp_width != -1)
			gmp->set_size_request(gmp_width, gmp_height);
		else
			gmp->set_size_request(width, height);
	}
	else {
		play_frame->remove(*gmp);
		play_frame->pack_start(*backgroup, true, true);
		backgroup->show_all();
		set_gmp_size(-1, -1);
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
	if (lp != NULL) {
		gmp->stop();
		live_player = lp;
		lp->signal_status().connect(sigc::mem_fun(
					*this, &MainWindow::on_live_player_out));
		lp->play(*gmp);
	} else {
		gmp->play();
	}
}

