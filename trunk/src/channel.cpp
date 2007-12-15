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
#include "MainWindow.h"
#include "live_player.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"

Channel::Channel(MainWindow* parent_):parent( parent_)
{
	Channel* channel = this;
	channel->set_flags(Gtk::CAN_FOCUS);
	channel->set_rules_hint(false);

	m_liststore = Gtk::TreeStore::create(columns);
	channel->set_model( m_liststore);
	channel->append_column("频道", columns.name);
	channel->append_column("码率", columns.freq);
	channel->append_column("用户数", columns.users);
	channel->show();
}

Channel::~Channel()
{
}

Gtk::TreeModel::iterator Channel::getListIter(Gtk::TreeModel::
				Children children, const std::string& groupname)
{
	return find_if(children.begin(),
			children.end(),
			bind2nd(CompareChannel(columns),groupname));
}
Gtk::TreeModel::iterator Channel::addGroup(const Glib::ustring& group)
{
	Gtk::TreeModel::iterator iter = m_liststore->append();
	(*iter)[columns.name] = group;
	(*iter)[columns.type]=GROUP_CHANNEL;

	return iter;
}

bool Channel::on_button_press_event(GdkEventButton * ev)
{
	bool result = Gtk::TreeView::on_button_press_event(ev);

	Glib::RefPtr < Gtk::TreeSelection > selection =
	    this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return result;

	Gtk::TreeModel::Path path(iter);
	Gtk::TreeViewColumn * tvc;
	int cx, cy;
					/** get_path_at_pos() 是为确认鼠标是否在选择行上点击的*/
	if (!this->
	    get_path_at_pos((int) ev->x, (int) ev->y, path, tvc, cx, cy))
		return FALSE;
	if ((ev->type == GDK_2BUTTON_PRESS ||
	     ev->type == GDK_3BUTTON_PRESS)) {
		if(GROUP_CHANNEL != (*iter)[columns.type]){
			play_selection_iter(iter);
		}
		else {
			if(this->row_expanded(path))
				this->collapse_row(path);
			else{
				this->expand_row(path,false);
				this->scroll_to_row(path);
			}
		}


	} else if ((ev->type == GDK_BUTTON_PRESS)
			&& (ev->button == 3)) {
		if(GROUP_CHANNEL != (*iter)[columns.type])
			NULL;
		//parent->getMenu().popup(1,ev->time);
	}

}

void Channel::play_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;

	play_selection_iter(iter);

}


void Channel::record_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	LivePlayer* lp = get_player(stream, page);
	parent->set_live_player(lp);
	//	lp->record();
	//	RecentChannel* rc =
	//		dynamic_cast<RecentChannel*>(parent->get_recent_channel());
	//	if (this != rc)
	//		rc->saveLine(name,stream,page);
	//
}

void Channel::store_selection()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	BookMarkChannel* bc =
		dynamic_cast<BookMarkChannel*>(parent->get_bookmark_channel());
	if (this != bc)
		bc->saveLine(name,stream,page);

	RecentChannel* rc =
		dynamic_cast<RecentChannel*>(parent->get_recent_channel());
	if (this != rc)
		rc->saveLine(name,stream,page);
}


void Channel::play_selection_iter(Gtk::TreeModel::iterator& iter)
{
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];
	
	LivePlayer* lp = parent->get_live_player();
	if (NULL != lp) {
		if (lp->get_stream() == stream) {
			//parent->set_live_player(lp);
			return;
		}
	}

	delete lp;
	lp = get_player(stream, page);
	parent->set_live_player(lp);
	RecentChannel* rc =
		dynamic_cast<RecentChannel*>(parent->get_recent_channel());
	if (this != rc)
		rc->saveLine(name,stream,page);
}
