
#ifndef SOPCASTCHANNEL_H_
#define SOPCASTCHANNEL_H_
#include "channel.h"
class MainWindow;
class SopcastChannel:public Channel
{
	public:
		SopcastChannel(MainWindow* parent_);
		void init();
		void  addLine(const int id,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname);
	private:
		MainWindow* parent;
	protected:
		bool on_button_press_event(GdkEventButton *);
};







#endif
