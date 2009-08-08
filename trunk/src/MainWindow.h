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
#include "TrayIcon.h"


class Channel;
class LivePlayer;
class ConfWindow;
class MainWindow : public Gtk::Window {
	public:
		MainWindow();
		~MainWindow();
		void on_conf_window_close(ConfWindow* dlg);
		bool on_key_press_event(GdkEventKey* ev);
		bool support_pplive()const {return enable_pplive;}
		bool support_sopcast()const {return enable_sopcast;}
		void hide_window();
		void show_window();
		Gtk::Menu* get_try_pop_menu() { return try_pop_menu; }

	protected:
		bool on_delete_event(GdkEventAny* event);
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
		void on_menu_view_hide_toolbar();
		void on_menu_view_always_on_top();
		void on_menu_view_embed_mplayer();
		void on_menu_view_preferences();
		void on_menu_help_about();
		//void on_menu_pop_refresh_list();
		void on_menu_pop_add_to_bookmark();
		void on_menu_pop_copy_to_clipboard();

		/** 查找列表按钮回调*/
		void on_search_channel();
		/** 更新频道列表回调*/
		void on_refresh_channel();
		void on_preview(const std::string& filename);
		/** 根据界面大小更新mplayer画面的大小*/
		void on_update_video_widget();
		void on_drog_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
				int, int, const Gtk::SelectionData& selection_data,
				guint,guint time);

		void on_live_player_out(int percentage);
		bool on_gmplayer_out(const Glib::IOCondition& condition);
		void on_gmplayer_start();
		void on_gmplayer_stop();
		/** 属性窗口退出处理*/
		void on_conf_window_quit();
		/** 双击画面事件*/
		bool on_doubleclick_picture(GdkEventButton* ev);
		/*
		void on_volume_change();
		Glib::ustring on_volume_display(double);
		*/
		/** 切换全屏状态,嵌入播放器时有效*/
		void on_fullscreen();
		void unzoom(); //解除全屏

		/**设置画面的大小，嵌入播放器时有效*/
		void set_gmp_size(int w, int h);
		/** 设置是否为嵌入播放器*/
		void set_gmp_embed(bool);
		void set_channels_hide(bool);
		void set_toolbar_hide(bool);
		/** 设置是否使用其它播放器*/
		void set_other_player(bool oplayer);

		void reorder_widget(bool is_running);
		Channel* get_cur_select_channel();
		/** 检测是否存在某文件 */
		bool check_file(const char* name);
		/** 检测是否支持第三方程序 */
		void check_support();
	private:
		TrayIcon				*tray_icon;
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
		Gtk::Widget* menubar;
		Gtk::Widget* toolbar;
		Gtk::Statusbar* 		statusbar;
		//Gtk::HBox* tool_hbox;
		Gtk::Image*			background;
		//Gtk::Adjustment*		adj_sound;
		Channel* 			recent_channel;
		Channel* 			bookmark_channel;
		Gtk::Menu*			channels_pop_menu;
		Gtk::Menu*			try_pop_menu;
		LivePlayer* 		        live_player;
		ConfWindow*			confwindow;
		int 				gmp_width;
		int				gmp_height;
		double				gmp_rate;	//mplayer画面比例
		int				window_width;
		int				window_height;
		int				window_x;
		int				window_y;
		bool				gmp_embed;	//是否嵌入mplayer
		bool				channels_hide;
		bool				toolbar_hide;
		bool				enable_pplive;
		bool				enable_sopcast;
		bool				enable_tray;
		bool				full_screen;
		bool				refresh_sopcast_channels; //是否每次启动程序都去下载sopcast的频道列表
		bool				refresh_pplive_channels; //是否每次启动程序都去下载pplive 的频道列表
};

#endif // _MAINWINDOW_HH 

