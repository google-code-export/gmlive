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
#include "sopcastchannel.h"
#include "pplivechannel.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"
#include "livePlayer.h"

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
#include <gtkmm/scalebutton.h>

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

		DLOG("w = %d, h = %d\n", w, h);
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
"   		<menuitem action='FullScreen'/>"
"			<menuitem action='Mute'/>"
"			<separator/>"
"			<menuitem action='FileQuit'/>"
"        	</menu>"
"		<menu action='ViewMenu'>"
"			<menuitem action='ViewEmbedMplayer'/>"
"			<menuitem action='ViewShowChannel'/>"
"			<menuitem action='ViewShowToolbar'/>"
"			<menuitem action='FullScreen'/>"
"			<menuitem action='AlwaysOnTop'/>"
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
"	<popup name='TryPopupMenu'>"
"		<menuitem action='FilePlay'/>"
"		<menuitem action='FilePause'/>"
"		<menuitem action='FileRecord'/>"
"		<menuitem action='FileStop'/>"
"		<menuitem action='Mute'/>"
"		<separator/>"
"		<menuitem action='ViewPreferences'/>"
"		<separator/>"
"		<menuitem action='FileQuit'/>"
"	</popup>"
"	<toolbar name='ToolBar'>"
"		<toolitem action='FilePlay'/>"
"		<toolitem action='FilePause'/>"
"		<toolitem action='FileRecord'/>"
"		<toolitem action='FileStop'/>"
"		<separator/>"
"		<toolitem action='FullScreen'/>"
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
	Gtk::Stock::add(Gtk::StockItem(stock_id, _("_HideChannels")));

	//add fullscreen icons to iconset
	//Gtk::IconSource source_fullscreen;
	//source_fullscreen.set_pixbuf( Gdk::Pixbuf::create_from_file(DATA_DIR"/fullscreen.png"));
	//source_fullscreen.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	//source_fullscreen.set_size_wildcarded(); // Icon may be scaled.
	//Gtk::IconSet icon_set_fullscreen;
	//icon_set_fullscreen.add_source(source_fullscreen);

	//const Gtk::StockID stock_id_fullscreen("FullScreen");
	//factory->add(stock_id_fullscreen,icon_set_fullscreen);
	//Gtk::Stock::add(Gtk::StockItem(stock_id_fullscreen,_("FullScreen")));

	//添加openURL图标进图标库
	Gtk::IconSource source_openurl;
	source_openurl.set_pixbuf( Gdk::Pixbuf::create_from_file(DATA_DIR"/OpenURL.png") );
	source_openurl.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	source_openurl.set_size_wildcarded(); //Icon may be scaled.
	Gtk::IconSet icon_set_openurl;
	icon_set_openurl.add_source(source_openurl); //More than one source_open_url per set is allowed.

	const Gtk::StockID stock_id_openurl("OpenURL");
	factory->add(stock_id_openurl, icon_set_openurl);
	Gtk::Stock::add(Gtk::StockItem(stock_id_openurl, _("Open_URL")));
	factory->add_default();
}
void MainWindow::init_ui_manager()
{
	register_stock_items();
	if (!action_group)
		action_group = Gtk::ActionGroup::create();
	Glib::RefPtr<Gtk::Action> action ;
	//File menu:
	action_group->add(Gtk::Action::create("FileMenu", _("_File")));

	action = Gtk::Action::create("OpenFile", Gtk::Stock::OPEN,_("_Open local file"));
	action->set_tooltip(_("Open Local File"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_open_file));
	action = Gtk::Action::create("OpenURL", Gtk::StockID("OpenURL"),_("Open_URL"));
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

	//action = Gtk::Action::create("FullScreen", Gtk::StockID("FullScreen"),_("FullScreen"));
	action = Gtk::Action::create("FullScreen", Gtk::Stock::FULLSCREEN);
	action->set_tooltip(_("FullScreen"));
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_fullscreen));

	action_group->add(Gtk::ToggleAction::create("Mute",
				_("_Mute"), _("_Mute"), false), 
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

	action = Gtk::ToggleAction::create("ViewShowToolbar", 
			_("Hide_Toolbar"), _("Show or hide Toolbar"), false);
	action_group->add(action,
			sigc::mem_fun(*this, &MainWindow::on_menu_view_hide_toolbar));
	
	action_group->add(Gtk::ToggleAction::create("AlwaysOnTop",
				_("_AlwaysOnTop"), _("Make GMlive ontop of other windows"), false), 
			sigc::mem_fun(*this, &MainWindow::on_menu_view_always_on_top));


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
			sigc::mem_fun(*this, &MainWindow::on_refresh_channel));
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
		Glib::ustring filtername = Glib::ustring("\"")+filename+"\"";
		DLOG("播放 %s\n",filtername.c_str());

		std::string& cache = GMConf["mms_mplayer_cache"];
		int icache = atoi(cache.c_str());
		icache = icache > 64 ? icache : 64;

		gmp->set_cache(icache);
		gmp->set_record(false);
		gmp->start(filtername);

	}
}

void MainWindow::on_menu_open_url()
{
	Gtk::Dialog dlg(_("Open URL"),*this,true);
	dlg.set_size_request(300,90);
	dlg.set_resizable(false);
	Gtk::VBox* m_vbox=dlg.get_vbox();
	Gtk::Entry m_entry;
	m_vbox->pack_start(m_entry);

	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	dlg.show_all_children();

	if (Gtk::RESPONSE_OK == dlg.run()) {
		Glib::ustring filename = m_entry.get_text();
		if (filename.empty())
			return;
		Glib::ustring filtername = Glib::ustring("\"")+filename+"\"";
		DLOG("播放 %s\n",filtername.c_str());
		std::string& cache = GMConf["mms_mplayer_cache"];
		int icache = atoi(cache.c_str());
		icache = icache > 64 ? icache : 8192;

		gmp->set_cache(icache);
		gmp->set_record(false);
		gmp->start(filtername);
	}

}


void MainWindow::on_menu_file_play()
{
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->play_selection();
	else
		DLOG("Error");

}

void MainWindow::on_menu_file_stop()
{
	gmp->stop();
}

void MainWindow::unzoom()
{
	if(full_screen && gmp_embed)
	{
		menubar->show();
		toolbar->show();
		//tool_hbox->show();
		statusbar->show();
		if(!channels_hide)
			channels_box->show();
		this->unfullscreen();
		full_screen=false;
	}


}
void MainWindow::on_fullscreen()
{
	if(!full_screen && gmp_embed)
	{
		/** 设置全屏状态*/
		menubar->hide();
		toolbar->hide();
		//tool_hbox->hide();
		statusbar->hide();
		channels_box->hide();
		this->fullscreen();
		full_screen=true;
	}
	else if(full_screen && gmp_embed)
	{

		/**解除全屏状态*/
		menubar->show();
		toolbar->show();
		//tool_hbox->show();
		statusbar->show();
		if(!channels_hide)
			channels_box->show();
		this->unfullscreen();
		full_screen=false;
	}
}
void MainWindow::on_menu_file_pause()
{
	DLOG("on_menu_pause\n");
	gmp->pause();
}

void MainWindow::on_menu_file_record()
{
	DLOG("on_menu_file_record");
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->record_selection();
	else
		DLOG("Error");
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
	DLOG("on_menu_file_quit");
	this->get_size( window_width, window_height);
	gmp->stop();
	Gtk::Main::quit();
}
/*
   void MainWindow::on_menu_pop_refresh_list()
   {
   Channel* channel = get_cur_select_channel();
   if (channel)
   channel->refresh_list();
   else
   DLOG("Error");

   }
   */

void MainWindow::on_menu_pop_add_to_bookmark()
{
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->store_selection();
	else
		DLOG("Error");
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

void MainWindow::show_window()
{
	this->show();
	this->move(window_x, window_y);
}

void MainWindow::hide_window()
{
	if (gmp_embed || !atoi(GMConf["close_to_systray"].c_str())) {
		gmp->stop();
	}
	this->get_position(window_x, window_y);
	this->hide();
}

bool MainWindow::on_delete_event(GdkEventAny* event)
{
	this->get_size( window_width, window_height);
	if(!atoi(GMConf["close_to_systray"].c_str()))
		on_menu_file_quit();
	else if (!atoi(GMConf["enable_tray"].c_str())) 
		on_menu_file_quit();
	else if (gmp_embed)
		gmp->stop();
	this->get_position(window_x, window_y);
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

void MainWindow::on_menu_view_hide_toolbar()
{
	//cout << "on_menu_view_hide_toolbar" << endl;

	Glib::RefPtr<Gtk::ToggleAction> show = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewShowToolbar"));

	if (show->get_active()){
		toolbar_hide = true;
		toolbar->hide();
	} else {
		toolbar_hide = false;
		toolbar->show();
	}
	this->resize(1, 1);
}

void MainWindow::on_menu_view_always_on_top()
{
	Glib::RefPtr<Gtk::ToggleAction> show = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("AlwaysOnTop"));

	if (show->get_active()){
		this->set_keep_above(true);
	} else {
		this->set_keep_above(false);
	}
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
	BuilderXML about_xml = Gtk::Builder::create_from_file(main_ui, "aboutwindow");
	Gtk::Window* about = 0;
	about_xml->get_widget("aboutwindow", about);
	about->show();
}

void MainWindow::on_search_channel()
{
	Channel* channel = get_cur_select_channel();
	if (!channel) 
		DLOG("Error");
	Gtk::Entry* search = 0;
	ui_xml->get_widget("entry_find_channel", search);

	if (search)
		channel->search_channel(search->get_text());
}

void MainWindow::on_refresh_channel()
{
	Channel* channel = get_cur_select_channel();
	if (channel)
		channel->refresh_list();
	else
		DLOG("Error");
}

bool MainWindow::on_doubleclick_picture(GdkEventButton* ev)
{

	if (ev->type == GDK_2BUTTON_PRESS)
	{
		on_fullscreen();
	}
}

#if 0
Glib::ustring MainWindow::on_volume_display(double value)
{
	char message[128];
	sprintf(message,"%d%%",(int)value);

	return Glib::ustring(message);
}
#endif

void MainWindow::on_conf_window_quit()
{
	//std::cout << "on_conf_window_quit" << std::endl;
	//set_gmp_embed(atoi(GMConf["mplayer_embed"].c_str()));
	set_other_player(atoi(GMConf["player_type"].c_str()));
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
						gmp_rate = (double)h/w;
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
	,gmp_rate(-1)
	,gmp_embed(true)
	,channels_hide(false)
	,toolbar_hide(false)
	,refresh_sopcast_channels(true)
	,enable_pplive(true)
	,enable_sopcast(true)
	,full_screen(false)
	,window_width(1)
	,window_height(1)
	,confwindow(NULL)
	,menubar(NULL)
	,toolbar(NULL)
	,enable_tray(false)
	,tray_icon(NULL)
	//,tray_icon(*this)
{
	std::list<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));

	ui_xml = Gtk::Builder::create_from_file(main_ui, "mainFrame");
	if (!ui_xml) 
		exit(127);


	Gtk::VBox* main_frame = 0;
	ui_xml->get_widget("mainFrame", main_frame);

	statusbar = 0;
	ui_xml->get_widget("statusbar", statusbar);

	play_frame = 0;
	ui_xml->get_widget("playFrame", play_frame);

	channels = 0;
	ui_xml->get_widget("channels", channels);

	channels_box = 0;
   	ui_xml->get_widget("channels_box", channels_box);
	//channels_box->set_size_request(120,100); //set channels box size

	Channel* channel = Gtk::manage(new class MMSChannel(this));
	Gtk::ScrolledWindow* swnd = 0;
	ui_xml->get_widget("mmsChannelWnd", swnd);
	swnd->add(*channel);

	init();
	/** 检测是否支持pplive和sopcast */
	check_support();
	refresh_sopcast_channels = atoi(GMConf["check_refresh_sopcast_channels"].c_str());

	if(enable_sopcast)
	{
		channel = Gtk::manage(new SopcastChannel(this));
		swnd = 0;
		ui_xml->get_widget("sopcastChannelWnd", swnd);
		swnd->add(*channel);
		DLOG("support sopcast\n");
		if(refresh_sopcast_channels)
			channel->refresh_list();
	}
	else
		channels->remove_page(1);

	if(enable_pplive)
	{
		channel = Gtk::manage(new class PpliveChannel(this));
		swnd = 0;
		ui_xml->get_widget("ppliveChannelWnd", swnd);
		swnd->add(*channel);
		DLOG("support pplive\n");
		if (refresh_pplive_channels)
			channel->refresh_list();
	}
	else
	{
		if(enable_sopcast)
			channels->remove_page(2);
		else
			channels->remove_page(1);
	}


	recent_channel = Gtk::manage(new class RecentChannel(this));
	swnd = 0;
	ui_xml->get_widget("recentChannelWnd", swnd);
	swnd->add(*recent_channel);

	bookmark_channel = Gtk::manage(new class BookMarkChannel(this));
	swnd = 0;
	ui_xml->get_widget("bookmarkChannelWnd", swnd);
	swnd->add(*bookmark_channel);


	background = new Gtk::Image(
			Gdk::Pixbuf::create_from_file (
				DATA_DIR"/gmlive_play.png"));

	gmp = new GMplayer();	
	gmp->set_out_slot(sigc::mem_fun(*this, &MainWindow::on_gmplayer_out));
	gmp->signal_start().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_start));
	gmp->signal_stop().connect(
			sigc::mem_fun(*this, &MainWindow::on_gmplayer_stop));

	init_ui_manager();
	menubar = ui_manager->get_widget("/MenuBar");
	toolbar = ui_manager->get_widget("/ToolBar");
	//Gtk::Widget* menubar = ui_manager->get_widget("/MenuBar");
	//Gtk::Widget* toolbar = ui_manager->get_widget("/ToolBar");
	channels_pop_menu = dynamic_cast<Gtk::Menu*>(
			ui_manager->get_widget("/PopupMenu"));

	try_pop_menu = dynamic_cast<Gtk::Menu*>(
			ui_manager->get_widget("/TryPopupMenu"));

	Gtk::VBox* menu_tool_box = 0;
	ui_xml->get_widget("box_menu_toolbar", menu_tool_box);
	menu_tool_box->pack_start(*menubar,true,true);
	//menu_tool_box->pack_start(*toolbar,false,false);

	Gtk::HBox* hbox_la=0;
	ui_xml->get_widget("hbox_la",hbox_la);


	menu_tool_box->pack_start(*hbox_la,false,false);
	hbox_la->pack_start(*toolbar,true,true);

#if  0
	tool_hbox=dynamic_cast<Gtk::HBox*>
		(ui_xml->get_widget("controlHbox"));
	Glib::RefPtr<Gdk::Pixbuf> icon_ = Gdk::Pixbuf::create_from_file(DATA_DIR"/volume.png");
	Gtk::Image* volume_icon= Gtk::manage(new Gtk::Image(icon_));
	adj_sound = Gtk::manage(new Gtk::Adjustment(60,0,100,5,0,0));
	Gtk::HScale* hscale=Gtk::manage(new Gtk::HScale(*adj_sound));
	tool_hbox->pack_end(*hscale,false,false);
	tool_hbox->pack_end(*volume_icon, false,false);
	hscale->set_size_request(120,0);
	hscale->signal_format_value().connect(sigc::
			mem_fun(*this,&MainWindow::on_volume_display));

	adj_sound->signal_value_changed().connect(sigc::
			mem_fun(*this,&MainWindow::on_volume_change));
#endif

	play_eventbox = Gtk::manage(new Gtk::EventBox());
	play_eventbox->set_events(Gdk::BUTTON_PRESS_MASK);
	play_eventbox->signal_button_press_event().connect(sigc::
			mem_fun(*this, &MainWindow::on_doubleclick_picture));
	play_eventbox->add(*background);
	play_eventbox->modify_bg(Gtk::STATE_NORMAL,Gdk::Color("black"));

	play_frame->pack_start(*play_eventbox,true,true);

	play_frame->drag_dest_set(listTargets);
	play_frame->signal_drag_data_received().connect(
			sigc::mem_fun(*this, &MainWindow::on_drog_data_received));
	this->signal_check_resize().connect(
			sigc::mem_fun(*this,&MainWindow::on_update_video_widget));


	this->add(*main_frame);

	Glib::RefPtr<Gdk::Pixbuf> pix = Gdk::Pixbuf::create_from_file(DATA_DIR"/gmlive.png");
	this->set_icon(pix);

	Gtk::Button* bt = 0;
	ui_xml->get_widget("bt_search_channel", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_search_channel));

	bt = 0;
	ui_xml->get_widget("bt_refresh_channel", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_refresh_channel));

	this->show_all();
	//channels->hide();
	this->resize(1,1);
	//init();
		
	Gtk::ScaleButton* vbt = 0;
	ui_xml->get_widget("av_sync_button", vbt);
	vbt->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_av_sync_change));

	vbt = 0;
	ui_xml->get_widget("volume_button", vbt);
	vbt->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_volume_change));

	if (atoi(GMConf["enable_tray"].c_str()))
		tray_icon = new TrayIcon(*this);

	const std::string& wnd_width = GMConf["main_window_width"];
	window_width = atoi(wnd_width.c_str());
	window_width = window_width > 0 ? window_width : 1;

	const std::string& wnd_height = GMConf["main_window_height"];
	window_height = atoi(wnd_height.c_str());
	window_height = window_height > 0 ? window_height : 1;

	channels_hide = atoi(GMConf["channels_hide"].c_str());
	toolbar_hide = atoi(GMConf["toolbar_hide"].c_str());
	gmp_embed     = atoi(GMConf["mplayer_embed"].c_str());

	set_channels_hide(channels_hide);
	set_toolbar_hide(toolbar_hide);
	//set_gmp_embed(gmp_embed);
	set_other_player(atoi(GMConf["player_type"].c_str()));
	//	Glib::RefPtr<Gtk::ToggleAction> menu = 
	//		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("Mute"));
	//	menu->set_active(false);
	((Gtk::Toolbar*)toolbar)->set_toolbar_style(Gtk::TOOLBAR_ICONS);


}

void MainWindow::on_volume_change(double var)
{
	char buf[64];
	snprintf(buf, 64, "volume %f 1\n", var * 100);
	gmp->send_ctrl_command(buf);
}

void MainWindow::on_av_sync_change(double sync)
{
	char buf[64];
	snprintf(buf, 64, "audio_delay %f 1\n", sync * 10 - 5);
	gmp->send_ctrl_command(buf);
}

void MainWindow::set_other_player(bool oplayer)
{
	if(oplayer){
		//使用其它播放器的情况，需要把界面设置成不嵌入模式的
		action_group->get_action("ViewEmbedMplayer")->set_sensitive(false);
		action_group->get_action("FilePause")->set_sensitive(false);
		set_gmp_embed(false);

	}
	else{
		//	action_group->get_action("ViewEmbedMplayer")->set_sensitive(true);

		Glib::RefPtr<Gtk::ToggleAction> menu = 
			Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewEmbedMplayer"));
		menu->set_sensitive(true);
		menu->set_active(atoi(GMConf["mplayer_embed"].c_str()));

		action_group->get_action("FilePause")->set_sensitive(true);
		set_gmp_embed(atoi(GMConf["mplayer_embed"].c_str()));

	}
	if (gmp->running())
		gmp->restart();

}
void MainWindow::set_gmp_embed(bool embed)
{
	//std::string& embed = GMConf["mplayer_embed"];
	//bool bembed = (!embed.empty()) && (embed[0] == '1');
	gmp_embed = embed;

	// 回写进配置中
	char buf[32];
	sprintf(buf, "%d", gmp_embed);
	GMConf["mplayer_embed"] = buf;
	// 这种写法太白痴了
	Glib::RefPtr<Gtk::ToggleAction> menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewEmbedMplayer"));
	menu->set_active(gmp_embed);
	if (!gmp_embed) {
		play_frame->hide();
		channels_box->show();
		action_group->get_action("ViewShowChannel")->set_sensitive(false);
		action_group->get_action("FullScreen")->set_sensitive(false);
		action_group->get_action("Mute")->set_sensitive(false);
		this->resize(window_width, window_height);
	}
	else {
		// 这里保存好channels的尺寸
		// 用于在不嵌入的时候恢复
		this->get_size( window_width, window_height);
		play_frame->show_all();
		action_group->get_action("ViewShowChannel")->set_sensitive(true);
		action_group->get_action("FullScreen")->set_sensitive(true);
		action_group->get_action("Mute")->set_sensitive(true);
		set_channels_hide(atoi(GMConf["channels_hide"].c_str()));
		this->resize(1, 1);
	}
	if (gmp->running())
		gmp->restart();
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
void MainWindow::set_toolbar_hide(bool hide)
{
	toolbar_hide = hide;
	if(toolbar_hide){
		toolbar->hide();
	}
	else{
		toolbar->show();
	}	
	Glib::RefPtr<Gtk::ToggleAction> menu = 
		Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action_group->get_action("ViewShowToolbar"));
	menu->set_active(toolbar_hide);

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

	save_conf();
}
bool MainWindow::check_file(const char* name)
{
	char *env = getenv("PATH");
	char buf[512];

	char *start=env, *end=env;

	// iterator over $PATH to find the program	
	while (*end != '\0'){
		memset(buf, '\0', 512);
		if (*(end +1) == ':' || *(end + 1) == '\0'){
			strncpy ( buf, start, end - start+1);
			if (buf[strlen(buf)-1] != '/')
				buf[strlen(buf)] = '/';

			strcat(buf, name);
			FILE* fp;
			fp = fopen(buf,"r");
			if(fp)
			{
				fclose(fp);
				return true;
			}

			start = end+2;
		}

		end++;
	}
	return false;
}
void MainWindow::check_support()
{
	std::string pplive_cmd("xpplive");
	std::string sopcast_cmd("sp-sc-auth");

	enable_sopcast=check_file(sopcast_cmd.c_str());
	enable_pplive=check_file(pplive_cmd.c_str());

	std::string& enablesopcast_ = GMConf["enable_sopcast"];
	std::string& enablepplive_ = GMConf["enable_pplive"];
	if(enablesopcast_[0]=='1'|enablepplive_[0]=='1')
		if(!enable_pplive| !enable_sopcast)
		{
			Gtk::MessageDialog warnDialog(_("NO SUPPORT"),
					false);
			std::string msg="";
			if((!enable_sopcast && (enablesopcast_[0]=='1'))&&(!enable_pplive && (enablepplive_[0]=='1')))
				msg+=std::string(_("you have not install sopcast and pplive program, so GMLive can't support them now"))+"\n";
			else if(!enable_sopcast && (enablesopcast_[0]=='1'))
				msg+=std::string(_("you have not install sopcast program,so GMLive can't support it now"))+"\n";
			else if(!enable_pplive && (enablepplive_[0]=='1'))
				msg+=std::string(_("you have not install pplive program,so GMLive can't support it now"))+"\n";
			else
				return;
			msg+=_(" So you can install the program first");
			warnDialog.set_secondary_text(msg);

			warnDialog.run();

		}
	if(enablepplive_[0]=='0')
		enable_pplive =0;
	if(enablesopcast_[0]=='0')
		enable_sopcast=0;


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
		GMConf["player_type"]="0"; //0--mplayer, 1--other player
		GMConf["other_player_cmd"]="";
		GMConf["mplayer_embed"]		=	"1";
		GMConf["enable_sopcast"] = "1";
		GMConf["enable_pplive"]= "1";
		GMConf["mms_mplayer_cache"]     =       "8192";
		GMConf["sopcast_mplayer_cache"] =       "64";
		GMConf["pplive_mplayer_cache"]  =       "64";
		GMConf["pplive_delay_time"]     =       "5";
		GMConf["channels_hide"]		=	"0";
		GMConf["toolbar_hide"]		=	"0";
		GMConf["sopcast_channel_url"]	=	"http://channel.sopcast.com/gchlxml";
		GMConf["mms_channel_url"]    	=       "http://www.gooth.cn/mms.lst";
		GMConf["pplive_channel_url"]    =       "http://list.pplive.com/zh-cn/xml/new.xml";
		GMConf["check_refresh_sopcast_channels"] = "1";
		GMConf["check_refresh_pplive_channels"] = "1";
		GMConf["enable_tray"]		= "1";
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

	GMConf["channels_hide"] = channels_hide?"1":"0";
	GMConf["toolbar_hide"] = toolbar_hide?"1":"0";
	GMConf["mplayer_embed"] = gmp_embed?"1":"0";
	//GMConf["check_refresh_sopcast_channels"] = refresh_sopcast_channels?"1":"0";

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

	if (atoi(GMConf["enable_tray"].c_str()))
	{
		if(!tray_icon)
			tray_icon = new TrayIcon(*this);
	}else
	{
		if(tray_icon)
		{
			delete tray_icon;
			tray_icon=NULL;
		}
	}

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
			play_eventbox->remove();
			play_eventbox->add(*gmp);

			if (gmp_width != -1)
				gmp->set_size_request(gmp_width, gmp_height);
			else
				gmp->set_size_request(width, height);
		}
		else {
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
	if (lp == live_player && lp != NULL) {	// 同一个频道
		if (!gmp->running())
			gmp->start();
		return;
	}
	else {
		gmp->stop();
		delete live_player;
		live_player = NULL;
	}

	if (lp != NULL) {
		if (!name.empty())
			play_channel_name = name;
		live_player = lp;
		lp->signal_status().connect(sigc::mem_fun(
					*this, &MainWindow::on_live_player_out));
		lp->start(*gmp);
	} 
}



void MainWindow::on_update_video_widget()
{
	if(!full_screen && gmp_embed && gmp->running())
	{
		int n_width;
		int n_height;
		n_width=play_frame->get_width();
		int reqw;
		int reqh;
		gmp->get_size_request(reqw, reqh);
		if (n_width == reqw)
		{
			//printf("skip width\n");
			return;
		}
		n_height = (int)n_width *gmp_rate;
		set_gmp_size(n_width,n_height);

	}
}

void MainWindow::on_drog_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
		int, int, const Gtk::SelectionData& selection_data,
		guint,guint time)
{
	if((selection_data.get_length() >= 0)&&(selection_data.get_format()== 8))
	{
		context->drag_finish(false,false,time);
		std::string filename = wind_unescape_string(selection_data.get_text().c_str(), NULL);
		size_t pos = filename.find('\r');
		if (std::string::npos != pos)
			filename = filename.substr(0, pos);
		Glib::ustring filtername = Glib::ustring("\"")+filename+"\"";
		DLOG("播放 %s\n",filtername.c_str());
		gmp->start(filtername);

	}
}


bool MainWindow::on_key_press_event(GdkEventKey* ev)
{
	if(ev->type !=GDK_KEY_PRESS)
		return Gtk::Window::on_key_press_event(ev);

	switch(ev->keyval){
		case GDK_Escape:
			unzoom();
			break;
		case GDK_F11:
			on_fullscreen();
			break;
		default:
			return Gtk::Window::on_key_press_event(ev);

	}
	return true;
}

