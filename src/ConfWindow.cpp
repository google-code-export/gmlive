/*
 * =====================================================================================
 *
 *       Filename:  ConfWindow.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月17日 21时49分13秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */

#include "ConfWindow.h"
#include "MainWindow.h"

ConfWindow::ConfWindow(MainWindow * parent_):parent(parent_)
{
	GlademmXML vbox_xml =
	    Gnome::Glade::Xml::create(main_ui, "vbox_conf");
	Gtk::VBox * vBox = dynamic_cast < Gtk::VBox * >
	    (vbox_xml->get_widget("vbox_conf"));

	Gtk::Button* btcancel= dynamic_cast<Gtk::Button*>
		(vbox_xml->get_widget("button_cancel"));
	Gtk::Button* btsave = dynamic_cast<Gtk::Button*>
		(vbox_xml->get_widget("button_save"));


	btcancel->signal_clicked().connect(
			sigc::mem_fun(*this, &ConfWindow::on_button_cancel));
	btsave->signal_clicked().connect(
			sigc::mem_fun(*this, &ConfWindow::on_button_save));


	add(*vBox);
	set_transient_for(*parent);


	set_default_size(600, 400);
	set_title(_("GMLive Conf Window"));
	show_all();
}

void ConfWindow::on_button_save()
{
	//save();
	delete this;
}

void ConfWindow::on_button_cancel()
{
	delete this;
}

bool ConfWindow::on_key_press_event(GdkEventKey * ev)
{
	if (ev->type != GDK_KEY_PRESS)
		return Gtk::Window::on_key_press_event(ev);
	switch (ev->keyval) {
	case GDK_Escape:
		on_button_cancel();
		break;
	default:
		return Gtk::Window::on_key_press_event(ev);
	}
	return true;
}
