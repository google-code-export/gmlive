/*
 * =====================================================================================
 *
 *       Filename:  TrayIcon.cpp
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

#include "TrayIcon.h"
#include "MainWindow.h"

TrayIcon::TrayIcon(MainWindow& wnd) :
	main_wnd_(wnd)
{
	stat_ = Gtk::StatusIcon::create_from_file(DATA_DIR"/gmlive.png");
	if (!stat_)
		return;

	stat_->signal_activate ().connect(sigc::mem_fun(*this, &TrayIcon::active_main_wnd));
	stat_->signal_popup_menu().connect(sigc::mem_fun(*this, &TrayIcon::on_popup_menu));

	stat_->set_tooltip("gmlive");
}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::on_popup_menu(guint button, guint32 activate_time)
{
	Gtk::Menu* pop_menu = 
			main_wnd_.get_try_pop_menu();
		if (pop_menu)
			pop_menu->popup(button, activate_time);
}

void TrayIcon::active_main_wnd()
{
	if (main_wnd_.is_visible()) 
		main_wnd_.hide_window();
	else
		main_wnd_.show_window();
}
