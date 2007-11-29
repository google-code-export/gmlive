#ifndef CHANNEL_H__
#define CHANNEL_H__

#include <gtkmm.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeselect.h>



class Channel:public Gtk::TreeView
{
	public:
		Channel();
		~Channel();

	private:
	protected:
		bool on_button_press_event(GdkEventButton *);

};










#endif
