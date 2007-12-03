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

	m_liststore = Gtk::TreeStore::create(columns);
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

Gtk::TreeModel::iterator Channel::getListIter(Gtk::TreeModel::
				Children children, const std::string& groupname)
{

	Gtk::TreeModel::iterator listiter= children.begin();
	for( ; listiter!=children.end();listiter++)
	{
		if((*listiter)[columns.name] == groupname)
			return listiter;
	}
			return listiter;

			/*
	return find_if(children.begin(),
			children.end(),
			bind2nd(CompareChannel(columns),groupname));
			*/
}
Gtk::TreeModel::iterator Channel::addGroup(const Glib::ustring& group)
{
	Gtk::TreeModel::iterator iter = m_liststore->append();
	(*iter)[columns.name] = group;
	(*iter)[columns.type]=GROUP_CHANNEL;

	return iter;
}
