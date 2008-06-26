/*
 * =====================================================================================
 * 
 *       Filename:  playStream.h
 * 
 *    Description:  播放网络流 
 * 
 *        Version:  1.0
 *        Created:  2007年12月22日 20时40分35秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _PLAY_STREAM_H_
#define _PLAY_STREAM_H_

#include <stdlib.h>
#include <iostream>
#include <gtkmm.h>
#include <gtkmm/socket.h>
#include "gmplayer.h"

class PlayStream : public GMplayer, public Gtk::Socket {
	public:
		PlayStream();
		~PlayStream();
		void start(const std::string&);
		void start();
		void full_screen();
		void set_embed(bool f_embed);
		void set_other_player(bool f_oplayer);
	private:
		void on_mplayer_exit();
		void initialize();
		std::string	file;		/* filename (internal)*/
		guint32		xid;		/* X window handle (internal)*/
		bool		is_embed;      /* identify the mplayer is embed the main window */
		bool		is_oplayer;    /* identify the default player is mplayer or the other player . 0 = mplayer , 1 = other player */
};




#endif // _PLAY_STREAM_H_

