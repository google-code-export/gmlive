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
		void stop();
		void full_screen();
		void set_embed(bool embed_);
	private:
		void on_mplayer_exit();
		void initialize();
		std::string	file;		/* filename (internal)*/
		guint32		xid;		/* X window handle (internal)*/
		bool		is_embed;
};




#endif // _PLAY_STREAM_H_

