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
class GMplayer;
class BookMarkChannel:public Channel
{
	public:
		BookMarkChannel(MainWindow* parent_);
		void init();
		void addLine(const Glib::ustring& name,const std::string& sream,const Glib::ustring& type,const Glib::ustring& groupname);
		void saveLine(const Glib::ustring& name,const std::string& sream,TypeChannel type,const Glib::ustring& groupname);
	protected:
		LivePlayer* get_player(const std::string& stream,TypeChannel page);
};


#endif   /* ----- #ifndef BOOKMARKCHANNEL_FILE_HEADER_INC  ----- */

