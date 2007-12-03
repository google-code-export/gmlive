/*
 * =====================================================================================
 * 
 *       Filename:  bookmarkchannel.h
 * 
 *    Description:  书签（我的收藏) 
 * 
 *        Version:  1.0
 *        Created:  2007年12月03日 00时43分38秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  BOOKMARKCHANNEL_FILE_HEADER_INC
#define  BOOKMARKCHANNEL_FILE_HEADER_INC
#include "channel.h"
class MainWindow;
class BookMarkChannel:public Channel
{
	public:
		BookMarkChannel(MainWindow* parent_);
		void init();
		void  addLine(const int id,const Glib::ustring& name,const std::string& sream);
		void saveLine(const int id,const Glib::ustring& name,const std::string& sream);
	private:
		MainWindow* parent;
	protected:
		bool on_button_press_event(GdkEventButton *);
};


#endif   /* ----- #ifndef BOOKMARKCHANNEL_FILE_HEADER_INC  ----- */

