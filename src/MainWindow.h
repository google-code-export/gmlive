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
#include "gmplayer.h"


class Channel;
class LivePlayer;
class MainWindow : public Gtk::Window {
	public:
		MainWindow();
		~MainWindow();
		GMplayer& get_mplayer() { return *gmp; }
	protected:
		bool on_delete_event(GdkEventAny* event);
		void show_msg(const Glib::ustring& msg,unsigned int id=0);
		void init_ui_manager();
		void save_conf();
		void init();
	private:
		friend class Channel;
		void set_live_player(LivePlayer*);
		LivePlayer* get_live_player() { return live_player; }
		Channel* get_recent_channel() { return recent_channel; }
		Channel* get_bookmark_channel() { return bookmark_channel; }
		// 菜单回调
		void on_menu_file_play();
		void on_menu_file_pause();
		void on_menu_file_stop();
		void on_menu_file_quit();
		void on_menu_view_show_channel();
		void on_menu_view_embed_mplayer();
		void on_menu_view_preferences();
		void on_menu_help_about();

		void on_live_player_out(int percentage);
		bool on_gmplayer_out(const Glib::IOCondition& condition);
		void on_gmplayer_start();
		void on_gmplayer_stop();
		void on_conf_window_quit();

		void set_gmp_size(int w, int h);
		void set_gmp_embed();

		void reorder_widget(bool is_running);
		Channel* get_cur_select_channel();
	private:
		GlademmXML 			ui_xml;
		Glib::RefPtr<Gtk::UIManager>	ui_manager;
		Glib::RefPtr<Gtk::ActionGroup> 	action_group;
		GMplayer* 			gmp;
		Gtk::Box*			play_frame;
		Gtk::Notebook*			channels;
		Gtk::Statusbar* 		statusbar;
		Gtk::Image*			backgroup;
		Channel* 			recent_channel;
		Channel* 			bookmark_channel;
		LivePlayer* 			live_player;
		int 				gmp_width;
		int				gmp_height;
		bool				gmp_embed;
};

#endif // _MAINWINDOW_HH 

