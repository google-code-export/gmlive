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

#include "gmlive.h"
#include "playStream.h"
#include "recordStream.h"


class Channel;
class LivePlayer;
class ConfWindow;
class MainWindow : public Gtk::Window {
	public:
		MainWindow();
		~MainWindow();
		void on_conf_window_close(ConfWindow* dlg);
	protected:
		bool on_delete_event(GdkEventAny* event);
		bool on_key_press_event(GdkEventKey* ev);
		void show_msg(const Glib::ustring& msg,unsigned int id=0);
		void init_ui_manager();
		void save_conf();
		void init();
	private:
		friend class Channel;
		void set_live_player(LivePlayer*, const Glib::ustring& name = "");
		LivePlayer* get_live_player() { return live_player; }
		RecordStream* get_record_gmp() { return record_gmp; }
		PlayStream* get_play_gmp() { return gmp; }
		Gtk::Menu* get_channels_pop_menu() { return channels_pop_menu; }
		Channel* get_recent_channel() { return recent_channel; }
		Channel* get_bookmark_channel() { return bookmark_channel; }
		// 菜单回调
		void on_menu_open_file();
		void on_menu_open_url();
		void on_menu_file_play();
		void on_menu_file_pause();
		void on_menu_file_record();
		void on_menu_file_stop();
		void on_menu_file_mute();
		void on_menu_file_quit();
		void on_menu_view_hide_channel();
		void on_menu_view_embed_mplayer();
		void on_menu_view_preferences();
		void on_menu_help_about();
		void on_menu_pop_refresh_list();
		void on_menu_pop_add_to_bookmark();
		void on_menu_pop_copy_to_clipboard();

		void on_search_channel();
		void on_preview(const std::string& filename);
		void on_drog_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
				int, int, const Gtk::SelectionData& selection_data,
				guint,guint time);

		void on_live_player_out(int percentage);
		bool on_gmplayer_out(const Glib::IOCondition& condition);
		void on_gmplayer_start();
		void on_gmplayer_stop();
		void on_conf_window_quit();
		bool on_doubleclick_picture(GdkEventButton* ev);
		void on_fullscreen();
		void unzoom(); //解除全屏

		void set_gmp_size(int w, int h);
		void set_gmp_embed(bool);
		void set_channels_hide(bool);

		void reorder_widget(bool is_running);
		Channel* get_cur_select_channel();
	private:
		GlademmXML 			ui_xml;
		Glib::RefPtr<Gtk::UIManager>	ui_manager;
		Glib::RefPtr<Gtk::ActionGroup> 	action_group;
		Glib::ustring			play_channel_name;
		PlayStream* 			gmp;
		RecordStream* 			record_gmp;
		Gtk::Box*			play_frame;
		Gtk::EventBox*			play_eventbox;
		Gtk::Notebook*			channels;
		Gtk::Widget*			channels_box;
		Gtk::Widget*			menubar;
		Gtk::Widget*			toolbar;
		Gtk::Statusbar* 		statusbar;
		Gtk::Image*			background;
		Channel* 			recent_channel;
		Channel* 			bookmark_channel;
		Gtk::Menu*			channels_pop_menu;
		ConfWindow*			confwindow;
		LivePlayer* 			live_player;
		int 				gmp_width;
		int				gmp_height;
		int				window_width;
		int				window_height;
		bool				gmp_embed;
		bool				channels_hide;
		bool				full_screen;
};

#endif // _MAINWINDOW_HH 
