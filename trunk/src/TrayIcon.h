/*
 * =====================================================================================
 *
 *       Filename:  TrayIcon.h
 *
 *
 *        Version:  1.0
 *        Created:  2007年12月21日 11时11分53秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */
#ifndef _TRAY_ICON_H_
#define _TRAY_ICON_H_
#include <gtkmm.h>

class MainWindow;

class TrayIcon {
	public:
		TrayIcon(MainWindow& wnd);
		~TrayIcon();
	private:
		void on_popup_menu(guint button, guint32 activate_time);
		void active_main_wnd();
		
		MainWindow& main_wnd_;
		Glib::RefPtr<Gtk::StatusIcon> stat_; 
};

#endif // _TRY_ICON_H_

