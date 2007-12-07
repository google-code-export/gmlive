/*
 * =====================================================================================
 * 
 *       Filename:  nslivechannel.h
 * 
 *    Description:  support to nslive channel list
 * 
 *        Version:  1.0
 *        Created:  2007年12月02日 08时59分11秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  NSLIVECHANNEL_FILE_HEADER_INC
#define  NSLIVECHANNEL_FILE_HEADER_INC
#include "channel.h"
class GMplayer;
class NSLiveChannel:public Channel
{
	public:
		NSLiveChannel(MainWindow* parent_);
		LivePlayer* get_player(GMplayer& gmp, const std::string& stream);
		void init();
		void  addLine(const int users,const Glib::ustring& name,const std::string& sream,const Glib::ustring& groupname);
		void play_selection();
		void record_selection();
		void store_selection();
		void refresh_list();

		void wait_pid_exit(GPid pid, int);
		int genlist_pid;
		bool refresh;


};
#endif   /* ----- #ifndef NSLIVECHANNEL_FILE_HEADER_INC  ----- */

