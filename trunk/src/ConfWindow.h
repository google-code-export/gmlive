/*
 * =====================================================================================
 * 
 *       Filename:  confwindow.h
 * 
 *    Description:  config class for gmlive
 * 
 *        Version:  1.0
 *        Created:  2007年12月17日 21时25分41秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  CONFWINDOW_FILE_HEADER_INC
#define  CONFWINDOW_FILE_HEADER_INC

#include <libglademm/variablesmap.h>
#include "gmlive.h"
class MainWindow;
class ConfWindow: public Gtk::Window
{

	public:
		ConfWindow(MainWindow* parent_);
		~ConfWindow();

		void on_button_save();
		void on_button_cancel();
		bool on_key_press_event(GdkEventKey* ev);
		bool on_delete_event(GdkEventAny* event);
		void on_enable_tray_toggle();
		void on_select_mplayer();

		typedef sigc::signal<void> type_signal_quit;
		type_signal_quit signal_quit()
		{ return signal_quit_; }
	private:
		void save();
		void write_to_GMConf();
		type_signal_quit signal_quit_;
		MainWindow* parent;
		Gnome::Glade::VariablesMap* m_pVariablesMap;
		Glib::ustring m_paramter;
		Glib::ustring m_sopcast_cache;
		Glib::ustring m_mms_cache;
		Glib::ustring m_pplive_delay;
		Glib::ustring m_pplive_cache;
		Glib::ustring m_sopcast_channel;
		Glib::ustring m_pplive_channel;
		Glib::ustring m_mms_channel;
		Glib::ustring m_oplayer_cmd;
		Gtk::ToggleButton* enable_tray;
		Gtk::ToggleButton* check_close_tray; 
		Gtk::RadioButton* radio_player;
		Gtk::HBox* hbox2;

		bool		m_oplayer;
		bool		m_embed;
		bool		m_enable_pplive;
		bool		m_enable_sopcast;
		bool		m_check_refresh_sopcast_channels;
		bool		m_check_refresh_pplive_channels;
		bool		m_close_try;
		bool		m_enable_try;

};
#endif   /* ----- #ifndef CONFWINDOW_FILE_HEADER_INC  ----- */

