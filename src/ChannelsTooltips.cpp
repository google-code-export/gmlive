/*
 * =====================================================================================
 *
 *       Filename:  ChannelsTooltips.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年10月09日 20时07分21秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */

#include "ChannelsTooltips.h"
#include "channel.h"
//#include "pixmaps.h"

ChannelsTooltips::ChannelsTooltips(Channel* view):channel(view)
						  ,Window(Gtk::WINDOW_POPUP)
{
	this->set_decorated(false);
	this->set_skip_pager_hint(true);
	this->set_skip_taskbar_hint(true);
	this->set_name("gtk-tooltips");
	this->set_position(Gtk::WIN_POS_MOUSE);
	this->set_resizable(false);
	this->set_border_width(4);
	this->set_app_paintable(true);
	this->property_can_focus()=false;

	label = Gtk::manage(new Gtk::Label());
	label->set_line_wrap(true);
	label->set_alignment(0.5,0.5);
	label->set_use_markup(true);

	avatar = Gtk::manage(new Gtk::Image());
	Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox());
	Gtk::Image* logo=Gtk::manage(new Gtk::Image(DATA_DIR"/gmlive.png"));
	hbox->pack_start(*logo);
	hbox->pack_start(*label);
	hbox->pack_start(*avatar);
	hbox->show_all();
	add(*hbox);
	add_events(Gdk::POINTER_MOTION_MASK); 
	this->signal_expose_event().connect(sigc::mem_fun(*this,
				&ChannelsTooltips::on_expose_event));
	this->signal_button_press_event().connect(sigc::mem_fun(*this,
				&ChannelsTooltips::on_button_press_event),false);
	//this->signal_motion_notify_event().connect(sigc::mem_fun(
	//			*this,&ChannelsTooltips::on_motion_event),false);

}
bool ChannelsTooltips::on_expose_event(GdkEventExpose* ev)
{
	int width,height;
	this->get_size(width,height);
	//Gtk::Requisition req = this->size_request();
	Glib::RefPtr<Gtk::Style> style=this->get_style();
	Gtk::Widget* widget = dynamic_cast<Gtk::Widget*>(this);
	const Glib::RefPtr<Gdk::Window> window=this->get_window();
	const Gdk::Rectangle* area=0;
	style->paint_flat_box(window,Gtk::STATE_NORMAL,Gtk::SHADOW_OUT,
			*area,*widget, "tooltip",0,0,width,height);
	return Gtk::Window::on_expose_event(ev);
}
bool ChannelsTooltips::on_motion_event(GdkEventMotion* ev)
{
	//hideTooltip();
}
void ChannelsTooltips::hideTooltip()
{
	this->hide();
}


void ChannelsTooltips::showTooltip(GdkEventMotion* ev)
{
	int x_root,y_root, origY;
	int width,height;
	int x,y;
	int s_width,s_height;
	int currentX,currentY;
	channel->get_pointer(currentX,currentY);

	x_root=(int)ev->x_root;
	y_root=(int)ev->y_root;
	origY =(int)ev->y;
	this->get_size(width,height);
	s_width = this->get_screen()->get_width();
	s_height = this->get_screen()->get_height();

	x=x_root - width/2;
	if(currentY>=origY)
		y=y_root+24;
	else
		y=y_root+6;

	//check if over the screen
	if(x+width > s_width)
		x=s_width-width;
	else if(x<0)
		x=0;
	if(y+height > s_height)
		y = y-height-24;
	else if(y<0)
		y=0;

	this->move(x,y);
	this->show_all();

}

