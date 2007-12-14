/*
 * =====================================================================================
 *
 *       Filename:  StreamMenu.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月04日 20时52分28秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */
#include "StreamMenu.h"
#include "MainWindow.h"

StreamMenu::StreamMenu(MainWindow& wnd)
{
	this->items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-media-play")));
	Gtk::ImageMenuItem* itemPlay = (Gtk::ImageMenuItem*)&this->items().back();

	this->items().push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::StockID("gtk-media-record")));
	Gtk::ImageMenuItem* itemRecord = (Gtk::ImageMenuItem*)&this->items().back();


	Gtk::Image* image = Gtk::manage(new Gtk::Image(Gtk::StockID("gtk-add"),Gtk::ICON_SIZE_MENU));
	this->items().push_back(Gtk::Menu_Helpers::ImageMenuElem("收藏",*image));
	Gtk::ImageMenuItem* itemAdd = (Gtk::ImageMenuItem*)&this->items().back();

	image = Gtk::manage(new Gtk::Image(Gtk::StockID("gtk-refresh"),Gtk::ICON_SIZE_MENU));
	this->items().push_back(Gtk::Menu_Helpers::ImageMenuElem("刷新列表",*image));
	Gtk::ImageMenuItem* itemRefresh = (Gtk::ImageMenuItem*)&this->items().back();

	image = Gtk::manage(new Gtk::Image(Gtk::StockID("gtk-refresh"),Gtk::ICON_SIZE_MENU));
	this->items().push_back(Gtk::Menu_Helpers::ImageMenuElem("展开列表",*image));
	Gtk::ImageMenuItem* itemExpand = (Gtk::ImageMenuItem*)&this->items().back();
	image = Gtk::manage(new Gtk::Image(Gtk::StockID("gtk-refresh"),Gtk::ICON_SIZE_MENU));
	this->items().push_back(Gtk::Menu_Helpers::ImageMenuElem("折叠列表",*image));
	Gtk::ImageMenuItem* itemCollapse = (Gtk::ImageMenuItem*)&this->items().back();
	this->show_all();

//	itemPlay->signal_activate().connect(
//			sigc::mem_fun(wnd,&MainWindow::on_menu_play_activate));
//	itemRecord->signal_activate().connect(
//			sigc::mem_fun(wnd, &MainWindow::on_menu_record_activate));
//	itemAdd->signal_activate().connect(
//			sigc::mem_fun(wnd, &MainWindow::on_menu_add_activate));
//	itemRefresh->signal_activate().connect(
//			sigc::mem_fun(wnd, &MainWindow::on_menu_refresh_activate));
//	itemExpand->signal_activate().connect(
//			sigc::mem_fun(wnd, &MainWindow::on_menu_expand_activate));
//	itemCollapse->signal_activate().connect(
//			sigc::mem_fun(wnd, &MainWindow::on_menu_collapse_activate));

}
