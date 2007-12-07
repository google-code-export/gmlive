/*
 * =====================================================================================
 * 
 *       Filename:  mmschannel.h
 * 
 *    Description:  support ordinarily mms stream like mms://xxx
 * 
 *        Version:  1.0
 *        Created:  2007年12月02日 09时12分37秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  MMSCHANNEL_FILE_HEADER_INC
#define  MMSCHANNEL_FILE_HEADER_INC

#include "channel.h"
class GMplayer;
class MMSChannel:public Channel
{
	public:
		MMSChannel(MainWindow* parent_);
		~MMSChannel(){}
		void init();
		LivePlayer* get_player(GMplayer& gmp, const std::string&);
	protected:
		void  addLine(const int users,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname);

		void play_selection();
		void record_selection();
		void store_selection();
	//private:
	//	MainWindow* parent;
	//protected:
	//	bool on_button_press_event(GdkEventButton *);
};

#endif   /* ----- #ifndef MMSCHANNEL_FILE_HEADER_INC  ----- */

