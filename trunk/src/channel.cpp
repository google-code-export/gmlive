/*
 * =====================================================================================
 *
 *       Filename:  channel.cpp
 *
 *    Description:  频道列表
 *
 *        Version:  1.0
 *        Created:  2007年12月01日 19时25分31秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */


#include "channel.h"

Channel::Channel()
{
	Channel* channel = this;
	channel->set_flags(Gtk::CAN_FOCUS);
	channel->set_rules_hint(false);

	m_liststore = Gtk::ListStore::create(columns);
	channel->set_model( m_liststore);
	channel->append_column("频道", columns.name);
	channel->append_column("码率", columns.freq);
	//channel->append_column("channel", columns.name);
	//channel->append_column("freq", columns.freq);
	channel->show();
}

Channel::~Channel()
{
}


