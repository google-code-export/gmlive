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
#include "livePlayer.h"
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
		return false;
	// 这是为了可以正常搜索，把搜索文本清空才行，该死的Gtk团队，太笨了.
	search_channel_name.clear();

	if ((ev->type == GDK_2BUTTON_PRESS ||
	     ev->type == GDK_3BUTTON_PRESS) && ev->button != 3) {
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
		if(GROUP_CHANNEL == (*iter)[columns.type])
			return false;
		Gtk::Menu* pop_menu = 
				parent->get_channels_pop_menu();
		if (pop_menu)
			pop_menu->popup(ev->button, ev->time);
		return true;
	}
	return false;
}

void Channel::play_selection()
{
	printf("\nplay_selection\n");
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows()) {
		parent->set_live_player(NULL);
		return ;
	}

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

	Gtk::FileChooserDialog dlg(*parent,
		       	"选择文件", 
			Gtk::FILE_CHOOSER_ACTION_SAVE);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  	dlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	dlg.run();
	//LivePlayer* lp = get_player(stream, page);
	//parent->set_live_player(lp);
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
	printf("\nplay_selection_iter\n");
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	if(GROUP_CHANNEL == (*iter)[columns.type]) {
		parent->set_live_player(NULL);
		return;
	}

	LivePlayer* lp = parent->get_live_player();
	if (NULL != lp) {
		if (lp->get_stream() == stream) {
			parent->set_live_player(NULL);
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

void Channel::search_channel(const Glib::ustring& name_)
{
	if (name_.empty())
		return;
	Glib::RefPtr<Gtk::TreeModel> model = this->get_model();
	if (search_channel_name != name_)
		model->foreach_iter(sigc::mem_fun(*this, &Channel::on_clean_foreach));
	search_channel_name = name_;
	model->foreach_iter(sigc::mem_fun(*this, &Channel::on_foreach_iter));
}

bool Channel::on_foreach_iter(const Gtk::TreeModel::iterator& iter)
{
	const Glib::ustring& name = (*iter)[columns.name];
	size_t pos = name.find(search_channel_name, 0);
	if (Glib::ustring::npos != pos) {
		Glib::RefPtr<Gtk::TreeSelection> sel = this->get_selection();
		if ((*iter)[columns.searched])
			return false;
		Gtk::TreeModel::Path path(iter);
		this->expand_to_path(path);
		this->scroll_to_row (path);
		sel->select(iter);
		(*iter)[columns.searched] = true;
		return true;
	}
	return false;
}

bool Channel::on_clean_foreach(const Gtk::TreeModel::iterator& iter)
{
	(*iter)[columns.searched] = false;
	return false;
}
