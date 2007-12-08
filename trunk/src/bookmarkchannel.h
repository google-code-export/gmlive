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
		LivePlayer* get_player(GMplayer& gmp, const std::string& stream,TypeChannel page);
		void  addLine(const int users,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname);
		void saveLine(const Glib::ustring& name,const std::string& sream,TypeChannel type);
		void play_selection();
		void record_selection();
};


#endif   /* ----- #ifndef BOOKMARKCHANNEL_FILE_HEADER_INC  ----- */

