/*
 * =====================================================================================
 *
 *       Filename:  MainWindow.cpp
 *
 *    Description:  程序的主窗口
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "mmschannel.h"
#include "nslivechannel.h"
#include "sopcastchannel.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"
#include "livePlayer.h"
#include "playStream.h"

#include "MainWindow.h"
#include "ConfWindow.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <glib/gi18n.h>
#include <stdio.h>
#include <string.h>

using namespace std;



#define HEX_ESCAPE '%'
int hex_to_int (gchar c)
{
	return  c >= '0' && c <= '9' ? c - '0'
		: c >= 'A' && c <= 'F' ? c - 'A' + 10
		: c >= 'a' && c <= 'f' ? c - 'a' + 10
		: -1;
}

int unescape_character (const char *scanner)
{
	int first_digit;
	int second_digit;

	first_digit = hex_to_int (*scanner++);
	if (first_digit < 0) {
		return -1;
	}

	second_digit = hex_to_int (*scanner++);
	if (second_digit < 0) {
		return -1;
	}

	return (first_digit << 4) | second_digit;
}

/** 用于在拖放时得到的文件名的转码*/
std::string  wind_unescape_string (const char *escaped_string, 
		const gchar *illegal_characters)
{
	const char *in;
	char *out;
	int character;

	if (escaped_string == NULL) {
		return std::string();
	}

	//result = g_malloc (strlen (escaped_string) + 1);
	char result[strlen (escaped_string) + 1];

	out = result;
	for (in = escaped_string; *in != '\0'; in++) {
		character = *in;
		if (*in == HEX_ESCAPE) {
			character = unescape_character (in + 1);

			/* Check for an illegal character. We consider '\0' illegal here. */
			if (character <= 0
					|| (illegal_characters != NULL
						&& strchr (illegal_characters, (char)character) != NULL)) {
				return std::string();
			}
			in += 2;
		}
		*out++ = (char)character;
	}

	*out = '\0';
	assert (out - result <= strlen (escaped_string));
	return std::string(result);

}


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
"			<menuitem action='OpenFile'/>"
"			<menuitem action='OpenURL'/>"
"			<menuitem action='FilePlay'/>"
"			<menuitem action='FilePause'/>"
"			<menuitem action='FileRecord'/>"
"			<menuitem action='FileStop'/>"
"			<menuitem action='Mute'/>"
"			<separator/>"
"			<menuitem action='FileQuit'/>"
"        	</menu>"
"		<menu action='ViewMenu'>"
"			<menuitem action='ViewEmbedMplayer'/>"
"			<menuitem action='ViewShowChannel'/>"
"			<menuitem action='ViewPreferences'/>"
"		</menu>"
"		<menu action='HelpMenu'>"
"			<menuitem action='HelpAbout'/>"
"		</menu>"
"	</menubar>"
"	<popup name='PopupMenu'>"
"		<menuitem action='FilePlay'/>"
"		<menuitem action='FilePause'/>"
"		<menuitem action='FileRecord'/>"
"		<menuitem action='FileStop'/>"
"		<menuitem action='Mute'/>"
"		<separator/>"
"		<menuitem action='PopCopyUrl'/>"
"		<separator/>"
"		<menuitem action='PopRefreshList'/>"
"		<menuitem action='PopAddToBookmark'/>"
"	</popup>"
"	<toolbar name='ToolBar'>"
"		<toolitem action='FilePlay'/>"
"		<toolitem action='FilePause'/>"
"		<toolitem action='FileRecord'/>"
"		<toolitem action='FileStop'/>"
"		<separator/>"
"		<toolitem action='ViewShowChannel'/>"
"		<separator/>"
"	</toolbar>"
"</ui>";

void register_stock_items()
{
	Glib::RefPtr<Gtk::IconFactory> factory = Gtk::IconFactory::create();
	Gtk::IconSource source;
	//This throws an exception if the file is not found:
	source.set_pixbuf( Gdk::Pixbuf::create_from_file(DATA_DIR"/show_channels.png") );

	source.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	source.set_size_wildcarded(); //Icon may be scaled.

	Gtk::IconSet icon_set;
	icon_set.add_source(source); //More than one source per set is allowed.

	const Gtk::StockID stock_id("HideChannels");
	factory->add(stock_id, icon_set);
	Gtk::Stock::add(Gtk::StockItem(stock_id, _("HideChannels")));
	factory->add_default();

	//添加openURL图标进图标库
	Glib::RefPtr<Gtk::IconFactory> factory_ = Gtk::IconFactory::create();
	Gtk::IconSource source_openurl;
	source_openurl.set_pixbuf( Gdk::Pixbuf::create_from_file(DATA_DIR"/OpenURL.png") );
	source_openurl.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	source_openurl.set_size_wildcarded(); //Icon may be scaled.
	Gtk::IconSet icon_set_openurl;
	icon_set_openurl.add_source(source_openurl); //More than one source_open_url per set is allowed.

	const Gtk::StockID stock_id_openurl("OpenURL");
	factory->add(stock_id_openurl, icon_set_openurl);
	Gtk::Stock::add(Gtk::StockItem(stock_id_openurl, _("OpenURL")));
	factory_->add_default();
}
void MainWindow::init_ui_manager()
{
	register_stock_items();
	if (!action_group)
		action_group = Gtk::ActionGroup::create();
	Glib::RefPtr<Gtk::Action> action ;
	//File menu:
	action_group->add(Gtk::Action::create("FileMenu", _("_File")));

	action = Gtk::Action::create("OpenFile", Gtk::Stock::OPEN,_("Open local file"));
	action->set_tooltip(_("Open Local File"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_open_file));
	action = Gtk::Action::create("OpenURL", Gtk::StockID("OpenURL"),_("OpenURL"));
	action->set_tooltip(_("Open Network Stream"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_open_url));
	action = Gtk::Action::create("FilePlay", Gtk::Stock::MEDIA_PLAY);
	action->set_tooltip(_("Play"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_file_play));

	action = Gtk::Action::create("FilePause", Gtk::Stock::MEDIA_PAUSE);
	action->set_tooltip(_("Pause"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_file_pause));

	action = Gtk::Action::create("FileRecord", Gtk::Stock::MEDIA_RECORD);
	action->set_tooltip(_("Record"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_file_record));

	action = Gtk::Action::create("FileStop", Gtk::Stock::MEDIA_STOP);
	action->set_tooltip(_("Stop"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_file_stop));


	action_group->add(Gtk::ToggleAction::create("Mute",
				_("MUTE"), _("MUTE"), true), 
			sigc::mem_fun(*this, &MainWindow::on_menu_file_mute));

	action_group->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
			sigc::mem_fun(*this, &MainWindow::on_menu_file_quit));

	//View menu:
	action_group->add(Gtk::Action::create("ViewMenu", _("_View")));

	action = Gtk::ToggleAction::create("ViewShowChannel", 
			Gtk::StockID("HideChannels"));
	action->set_tooltip(_("Show or hide channels list"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_view_hide_channel));

	action_group->add(Gtk::ToggleAction::create("ViewEmbedMplayer",
				_("_Embed Mplayer"), _("Embed or Separate mplayer play"), true), 
			sigc::mem_fun(*this, &MainWindow::on_menu_view_embed_mplayer));

	action_group->add(Gtk::Action::create("ViewPreferences", Gtk::Stock::PREFERENCES),
			sigc::mem_fun(*this, &MainWindow::on_menu_view_preferences));

	//Help menu:
	action_group->add(Gtk::Action::create("HelpMenu", _("_Help")));
	action_group->add(Gtk::Action::create("HelpAbout", Gtk::Stock::HELP),
			sigc::mem_fun(*this, &MainWindow::on_menu_help_about));

	//Pop menu:
	action_group->add(Gtk::Action::create("PopRefreshList", 
				_("_Refresh list"), _("Refresh current channel list")),
			sigc::mem_fun(*this, &MainWindow::on_menu_pop_refresh_list));
	action_group->add(Gtk::Action::create("PopAddToBookmark", 
				_("_Add to bookmark"), _("Bookmark this channel")),
			sigc::mem_fun(*this, &MainWindow::on_menu_pop_add_to_bookmark));
	action_group->add(Gtk::Action::create("PopCopyUrl", 
				_("_Copy url"), _("Copy url to clipboard")),
			sigc::mem_fun(*this, &MainWindow::on_menu_pop_copy_to_clipboard));
				

	if (!ui_manager)
		ui_manager = Gtk::UIManager::create();

	ui_manager->insert_action_group(action_group);
	add_accel_group(ui_manager->get_accel_group());
	ui_manager->add_ui_from_string(ui_info);

	//return ui_manager->get_widget("/MenuBar");
}

void MainWindow::on_menu_open_file()
{
	Gtk::FileChooserDialog dlg(*this,
			_("Choose File"), 
			Gtk::FILE_CHOOSER_ACTION_SAVE);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//Gtk::FileFilter filter_media;
	//filter_media.set_name();
	if (Gtk::RESPONSE_OK == dlg.run()) {
		Glib::ustring filename = dlg.get_filename();
		if (filename.empty())
			return;
		std::cout<<"播放 "<<filename<<std::endl;
		gmp->start(filename);

	}
}

void MainWindow::on_menu_open_url()
{

}


void MainWindow::on_menu_file_play()
{
	//cout << "on_menu_file_play" << endl;
	//gmp->start("mms://61.139.37.135/star");
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->play_selection();
	else
		cout << "Error" << endl;

}

void MainWindow::on_menu_file_stop()
{
	//cout << "on_menu_stop" << endl;
	gmp->stop();
}

void MainWindow::on_menu_file_pause()
{
	//cout << "on_menu_pause" << endl;
	gmp->pause();
}

void MainWindow::on_menu_file_record()
{
	//cout << "on_menu_file_record" << endl;
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->record_selection();
	else
		cout << "Error" << endl;
}

void MainWindow::on_menu_file_mute()
{
	Glib::RefPtr<Gtk::ToggleAction> mute = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("Mute"));
	bool mute_=mute->get_active();

	gmp->mute(mute_);
}

void MainWindow::on_menu_file_quit()
{
	this->get_size( window_width, window_height);
	//cout << "on_menu_file_quit" << endl;
	gmp->stop();
	Gtk::Main::quit();
}

void MainWindow::on_menu_pop_refresh_list()
{
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->refresh_list();
	else
		cout << "Error" << endl;

}

void MainWindow::on_menu_pop_add_to_bookmark()
{
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->store_selection();
	else
		cout << "Error" << endl;
}

void MainWindow::on_menu_pop_copy_to_clipboard()
{
	Channel* channel = get_cur_select_channel();
	if (!channel) 
		return;
	Glib::ustring stream = channel->get_stream();
	if (stream.empty())
		return;
	Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
	clip->set_text(stream);
}

bool MainWindow::on_delete_event(GdkEventAny* event)
{
	this->get_size( window_width, window_height);
	gmp->stop();
	return Gtk::Window::on_delete_event(event);
}

void MainWindow::on_menu_view_hide_channel()
{
	//cout << "on_menu_view_hide_channel" << endl;

	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> show = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewShowChannel"));

	if (show->get_active()){
		channels_hide = true;
		channels_box->hide();
	} else {
		channels_hide = false;
		channels_box->show();
	}
	this->resize(1, 1);
}

void MainWindow::on_menu_view_embed_mplayer()
{
	//cout << "on_menu_view_embed_mplayer" << endl;
	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> embed = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewEmbedMplayer"));
	set_gmp_embed(embed->get_active());
}

void MainWindow::on_menu_view_preferences()
{
	if(NULL==confwindow)
	{
		confwindow = new ConfWindow(this);
		confwindow->signal_quit().connect(sigc::mem_fun(*this, &MainWindow::on_conf_window_quit));
	}
	else
		confwindow->raise();
}

void MainWindow::on_menu_help_about()
{
	GlademmXML about_xml = Gnome::Glade::Xml::create(main_ui, "aboutwindow");
	Gtk::Window* about = dynamic_cast<Gtk::Window*>
		(about_xml->get_widget("aboutwindow"));
	about->show();
}

void MainWindow::on_search_channel()
{
	Channel* channel = get_cur_select_channel();
	if (!channel) 
		cout << "Error" << endl;
	Gtk::Entry* search = dynamic_cast<Gtk::Entry*>(
			ui_xml->get_widget("entry_find_channel"));
	if (search)
		channel->search_channel(search->get_text());
}

bool MainWindow::on_doubleclick_picture(GdkEventButton* ev)
{

	if (ev->type == GDK_2BUTTON_PRESS)
	{
		printf(" 双击画面 \n");
	}
}

void MainWindow::on_volume_change()
{
	int value= (int)adj_sound->get_value();
	//printf("sound change to %d\n",value);
}
Glib::ustring MainWindow::on_volume_display(double value)
{
	char message[128];
	sprintf(message,"%d%%",(int)value);

	return Glib::ustring(message);
}

void MainWindow::on_conf_window_quit()
{
	//std::cout << "on_conf_window_quit" << std::endl;
	set_gmp_embed(atoi(GMConf["mplayer_embed"].c_str()));
	save_conf();
}

void MainWindow::on_conf_window_close(ConfWindow* dlg)
{
	g_assert(dlg == confwindow);
	delete confwindow;
	confwindow=NULL;
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
	char* pend = buf + len;
	char* p1 = buf;
	char* p2 = buf;
	for(; p1 < pend;) {
		if (!iscntrl(*p1)) {
			p1++;
		}
		else {
			*p1 = 0;
			show_msg(play_channel_name + Glib::ustring(p2, p1));
			if (-1 == gmp_width) {
				if ((p1 - p2) > 5) {
					int w,h;
					if (get_video_resolution(p2, w, h)) {
						set_gmp_size(w, h);
						show_msg(play_channel_name);
						return false;
					}
				}
			}

			for (p1++; p1 < pend; p1++) {
				if (!iscntrl(*p1))
					break;
			}
			p2 = p1;
		}
	}
	if (p1 != p2)
		show_msg(play_channel_name + Glib::ustring(p2, pend));
	return true;
}

void MainWindow::on_live_player_out(int percentage)
{
	char buf[256];
	sprintf(buf, _("Caching... %d%%"), percentage);
	show_msg(buf);
}

MainWindow::MainWindow():
	live_player(NULL)
	,gmp_width(-1)
	,gmp_height(-1)
	,gmp_embed(true)
	,window_width(1)
	,window_height(1)
{
	std::list<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));

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

	channels_box = ui_xml->get_widget("channels_box");

	Channel* channel = Gtk::manage(new class MMSChannel(this));
	Gtk::ScrolledWindow* swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("mmsChannelWnd"));
	swnd->add(*channel);

	init();
	/** 检测是否支持nslive和sopcast */
	check_support();

	if(enable_nslive)
	{
	channel = Gtk::manage(new class NSLiveChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("nsliveChannelWnd"));
	swnd->add(*channel);
	}

	if(enable_sopcast)
	{
	channel = Gtk::manage(new class SopcastChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("sopcastChannelWnd"));
	swnd->add(*channel);
	}

	recent_channel = Gtk::manage(new class RecentChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("recentChannelWnd"));
	swnd->add(*recent_channel);

	bookmark_channel = Gtk::manage(new class BookMarkChannel(this));
	swnd = dynamic_cast<Gtk::ScrolledWindow*>
		(ui_xml->get_widget("bookmarkChannelWnd"));
	swnd->add(*bookmark_channel);


	background = new Gtk::Image(
			Gdk::Pixbuf::create_from_file (
				DATA_DIR"/gmlive_play.png"));

	gmp = new PlayStream();	
	gmp->set_out_slot(sigc::mem_fun(*this, &MainWindow::on_gmplayer_out));
	gmp->signal_start().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_start));
	gmp->signal_stop().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_stop));

	record_gmp = new RecordStream();
	record_gmp->signal_preview().connect(
			sigc::mem_fun(*this,&MainWindow::on_preview));

	init_ui_manager();
	Gtk::Widget* menubar = ui_manager->get_widget("/MenuBar");
	Gtk::Widget* toolbar = ui_manager->get_widget("/ToolBar");
	channels_pop_menu = dynamic_cast<Gtk::Menu*>(
			ui_manager->get_widget("/PopupMenu"));

	Gtk::VBox* menu_tool_box = dynamic_cast<Gtk::VBox*>
		(ui_xml->get_widget("box_menu_toolbar"));
	menu_tool_box->pack_start(*menubar,true,true);
	menu_tool_box->pack_start(*toolbar,false,false);

	//Gtk::HBox* tool_hbox=Gtk::manage(new Gtk::HBox());
	Gtk::HBox* tool_hbox=dynamic_cast<Gtk::HBox*>
		(ui_xml->get_widget("controlHbox"));
	//menu_tool_box->pack_start(*tool_hbox);
	//tool_hbox->pack_start(*toolbar,true,true);
	//Gtk::VSeparator* vsparator=Gtk::manage(new Gtk::VSeparator());
	//tool_hbox->pack_start(*vsparator,false,false);
	//Gtk::Label* volume_label=Gtk::manage(new Gtk::Label(_("Volume")));
	Glib::RefPtr<Gdk::Pixbuf> icon_ = Gdk::Pixbuf::create_from_file(DATA_DIR"/volume.png");
	Gtk::Image* volume_icon= Gtk::manage(new Gtk::Image(icon_));
	adj_sound = Gtk::manage(new Gtk::Adjustment(60,0,100,5,0,0));
	Gtk::HScale* hscale=Gtk::manage(new Gtk::HScale(*adj_sound));
	tool_hbox->pack_end(*hscale,false,false);
	//tool_hbox->pack_end(*volume_label,false,false);
	tool_hbox->pack_end(*volume_icon, false,false);
	hscale->set_size_request(120,0);
	hscale->signal_format_value().connect(sigc::
			mem_fun(*this,&MainWindow::on_volume_display));

	adj_sound->signal_value_changed().connect(sigc::
			mem_fun(*this,&MainWindow::on_volume_change));

	play_eventbox = Gtk::manage(new Gtk::EventBox());
	play_eventbox->set_events(Gdk::BUTTON_PRESS_MASK);
	play_eventbox->signal_button_press_event().connect(sigc::
			mem_fun(*this, &MainWindow::on_doubleclick_picture));
	play_eventbox->add(*background);
	play_frame->pack_start(*play_eventbox,true,true);
	//play_frame->pack_start(*background, true, true);

	play_frame->drag_dest_set(listTargets);
	play_frame->signal_drag_data_received().connect(
			sigc::mem_fun(*this, &MainWindow::on_drog_data_received));


	this->add(*main_frame);

	Glib::RefPtr<Gdk::Pixbuf> pix = Gdk::Pixbuf::create_from_file(DATA_DIR"/gmlive.png");
	this->set_icon(pix);

	ui_xml->connect_clicked("bt_search_channel",
			sigc::mem_fun(*this, &MainWindow::on_search_channel));
	this->show_all();
	//channels->hide();
	this->resize(1,1);
	//init();
	
	const std::string& wnd_width = GMConf["main_window_width"];
	window_width = atoi(wnd_width.c_str());
	window_width = window_width > 0 ? window_width : 1;

	const std::string& wnd_height = GMConf["main_window_height"];
	window_height = atoi(wnd_height.c_str());
	window_height = window_height > 0 ? window_height : 1;

	channels_hide = atoi(GMConf["channels_hide"].c_str());
	gmp_embed     = atoi(GMConf["mplayer_embed"].c_str());

	set_channels_hide(channels_hide);
	set_gmp_embed(gmp_embed);
	Glib::RefPtr<Gtk::ToggleAction> menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("Mute"));
	menu->set_active(false);
	((Gtk::Toolbar*)toolbar)->set_toolbar_style(Gtk::TOOLBAR_ICONS);
	channel->refresh_list();

}

void MainWindow::set_gmp_embed(bool embed)
{
	//std::string& embed = GMConf["mplayer_embed"];
	//bool bembed = (!embed.empty()) && (embed[0] == '1');
	gmp_embed = embed;

	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewEmbedMplayer"));
	menu->set_active(gmp_embed);
	if (!gmp_embed) {
		play_frame->hide();
		channels_box->show();
		action_group->get_action("ViewShowChannel")->set_sensitive(false);
		this->resize(window_width, window_height);
	}
	else {
		// 这里保存好channels的尺寸
		// 用于在不嵌入的时候恢复
		this->get_size( window_width, window_height);
		play_frame->show_all();
		action_group->get_action("ViewShowChannel")->set_sensitive(true);
		set_channels_hide(channels_hide);
		this->resize(1, 1);
	}
	gmp->set_embed(gmp_embed);
}

void MainWindow::set_channels_hide(bool hide)
{
	Glib::RefPtr<Gtk::ToggleAction> embed_menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewEmbedMplayer"));
	channels_hide = hide;
	if(channels_hide){
		channels_box->hide();
	}
	else{
		channels_box->show();
	}	
	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewShowChannel"));
	menu->set_active(channels_hide);

}

MainWindow::~MainWindow()
{
	delete background;
	delete live_player;
	delete gmp;
	char buf[32];
	sprintf(buf, "%d", window_width);
	GMConf["main_window_width"] = buf;

	sprintf(buf, "%d", window_height);
	GMConf["main_window_height"] = buf;

	sprintf(buf, "%d", channels_hide);
	GMConf["channels_hide"] = buf;

	sprintf(buf, "%d", gmp_embed);
	GMConf["mplayer_embed"] = buf;

	save_conf();
}
bool MainWindow::check_file(const char* name)
{
	char filename[512]="/usr/bin/";
	char filename_local[512]="/usr/local/bin/";
	strcat(filename, name);
	strcat(filename_local, name);
	FILE* fp;
	fp = fopen(filename,"r");
	if(!fp)
	{
		fp=fopen(filename_local,"r");
		if(!fp)
		{
			return false;
		}
		fclose(fp);
		return true;
	}
	fclose(fp);
	return true;

}
void MainWindow::check_support()
{
	std::string nslive_cmd("nslive");
	std::string sopcast_cmd("sp-sc-auth");

	enable_sopcast=check_file(sopcast_cmd.c_str());
	enable_nslive=check_file(nslive_cmd.c_str());

	std::string& enablesopcast_ = GMConf["enable_sopcast"];
	std::string& enablenslive_ = GMConf["enable_nslive"];
	if(enablesopcast_[0]=='1'|enablenslive_[0]=='1')
	if(!enable_nslive| !enable_sopcast)
	{
		Gtk::MessageDialog warnDialog(_("NO SUPPORT"),
					false);
		std::string msg="";
		if(!enable_sopcast && (enablesopcast_[0]=='1'))
			msg+=std::string(_("you have not install sopcast program,so GMLive can't support it now"))+"\n";
		if(!enable_nslive && (enablenslive_[0]=='1'))
			msg+=std::string(_("you have not install nslive program,so GMLive can't support it now"))+"\n";
		msg+=_(" So you can install the program first");
		warnDialog.set_secondary_text(msg);

		warnDialog.run();

	}


}

void MainWindow::init()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/config",homedir);
	std::ifstream file(buf);
	if(!file){
		char homepath[512];
		snprintf(homepath,512,"%s/.gmlive/",homedir);
		mkdir(homepath,S_IRUSR|S_IWUSR|S_IXUSR);
		GMConf["mplayer_embed"]		=	"1";
		GMConf["enable_sopcast"] = "1";
		GMConf["enable_nslive"]= "1";
		GMConf["mms_mplayer_cache"]     =       "8192";
		GMConf["sopcast_mplayer_cache"] =       "64";
		GMConf["nslive_mplayer_cache"]  =       "64";
		GMConf["nslive_delay_time"]     =       "2";
		GMConf["channels_hide"]		=	"0";
		GMConf["sopcast_channel_url"]		=	"http://channel.sopcast.com/gchlxml";
		return;
	}
	std::string line;
	std::string name;
	std::string key;

	if(file){
		while(std::getline(file,line)){
			size_t pos= line.find_first_of("=");
			if(pos==std::string::npos)
				continue;
			name = line.substr(0,pos);
			key = line.substr(pos+1,std::string::npos);
			// 下面这2个把所有的空格都去掉了
			//key.erase(std::remove_if(key.begin(), key.end(), IsBlank()), key.end());
			//name.erase(std::remove_if(name.begin(), name.end(), IsBlank()), name.end());
			size_t pos1 = 0;
			size_t pos2 = 0;
			size_t len = 0;
			pos1 = name.find_first_not_of(" \t");
			pos2 = name.find_last_not_of(" \t");
			if (pos1 == std::string::npos || pos2 == std::string::npos)
				continue;
			len = pos2 - pos1 + 1;
			name = name.substr(pos1, len);

			pos1 = key.find_first_not_of(" \t");
			pos2 = key.find_last_not_of(" \t");
			if (pos1 == std::string::npos || pos2 == std::string::npos)
				continue;
			len = pos2 - pos1 + 1;
			key = key.substr(pos1, len);

			GMConf.insert(std::pair<std::string,std::string>(name,key));
		}
	}
	file.close();

	/*
	const std::string& wnd_width = GMConf["main_window_width"];
	window_width = atoi(wnd_width.c_str());
	window_width = window_width > 0 ? window_width : 1;

	const std::string& wnd_height = GMConf["main_window_height"];
	window_height = atoi(wnd_height.c_str());
	window_height = window_height > 0 ? window_height : 1;

	channels_hide = atoi(GMConf["channels_hide"].c_str());
	gmp_embed     = atoi(GMConf["mplayer_embed"].c_str());

	set_channels_hide(channels_hide);
	set_gmp_embed(gmp_embed);
	*/
}

void MainWindow::save_conf()
{
	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/.gmlive/config",homedir);
	std::ofstream file(buf);
	std::string line;
	std::map<std::string,std::string>::iterator iter=GMConf.begin();
	for(;iter != GMConf.end(); ++iter)
	{
		line = iter->first + "\t=\t" + iter->second;
		file << line << std::endl;
	}
	file.close();

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
		if (gmp_embed)
			this->resize(1, 1);
	}
}

void MainWindow::reorder_widget(bool is_running)
{
	if (!gmp_embed)
		play_frame->hide();
	else {
		if (is_running){
			static int width = background->get_width();
			static int height = background->get_height();
			//play_frame->remove(*background);
			//play_frame->pack_start(*gmp, true, true);
			//play_eventbox->remove(*background);
			play_eventbox->remove();
			play_eventbox->add(*gmp);

			if (gmp_width != -1)
				gmp->set_size_request(gmp_width, gmp_height);
			else
				gmp->set_size_request(width, height);
		}
		else {
			//play_frame->remove(*gmp);
			//play_frame->pack_start(*background, true, true);
			//play_eventbox->remove(*gmp);
			play_eventbox->remove();
			play_eventbox->add(*background);

			set_gmp_size(-1, -1);
		}
		play_frame->show_all();
		this->resize(1,1);
	}
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

void MainWindow::set_live_player(LivePlayer* lp, 
		const Glib::ustring& name)
{
	if (lp != NULL) {
		if (!name.empty())
			play_channel_name = name;
		live_player = lp;
		lp->signal_status().connect(sigc::mem_fun(
					*this, &MainWindow::on_live_player_out));
		lp->start(*gmp);
	} else {
		live_player = NULL;
		gmp->stop();
	}
}


void MainWindow::on_preview(const std::string& filename)
{
	gmp->start(filename);
}

void MainWindow::on_drog_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
		int, int, const Gtk::SelectionData& selection_data,
		guint,guint time)
{
	if((selection_data.get_length() >= 0)&&(selection_data.get_format()== 8))
	{
		context->drag_finish(false,false,time);
		std::string filename = wind_unescape_string(selection_data.get_text().c_str(), NULL);
		//std::cout << filename << std::endl;
		size_t pos = filename.find('\r');
		if (std::string::npos != pos)
			filename = filename.substr(0, pos);
		gmp->start(filename);
	}
}

