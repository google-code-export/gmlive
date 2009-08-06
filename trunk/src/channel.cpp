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


#include <sstream>
#include "channel.h"
#include "MainWindow.h"
#include "livePlayer.h"
#include "recentchannel.h"
#include "bookmarkchannel.h"
#include "ChannelsTooltips.h"
#include "recordStream.h"
#include <cassert>
#include <glib/gi18n.h>

Channel::Channel(MainWindow* parent_):parent( parent_)
{
	Channel* channel = this;
	channel->set_flags(Gtk::CAN_FOCUS);
	channel->set_rules_hint(false);

	tooltips = new ChannelsTooltips(this);
	m_liststore = Gtk::TreeStore::create(columns);
	channel->set_model( m_liststore);
	channel->append_column(_("channels"), columns.name);
	channel->append_column(_("bitrate"), columns.freq);
	channel->append_column(_("user"), columns.users);

	/*
	this->set_has_tooltip();
	this->set_tooltip_window(*tooltips);
	this->signal_query_tooltip().connect(sigc::mem_fun(*this,
				&Channel::on_tooltip_show));
	*/
	this->signal_motion_notify_event().
		connect(sigc::mem_fun(*this, &Channel::on_motion_event),
				false);
	this->signal_leave_notify_event().
		connect(sigc::mem_fun(*this, &Channel::on_leave_event),
				false);

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
	//printf("\nplay_selection\n");
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows()) {
		//parent->set_live_player(parent->get_live_player() ,"");
		return ;
	}
	if(GROUP_CHANNEL != (*iter)[columns.type])
		play_selection_iter(iter);

}


void Channel::record_selection()
{

	RecordStream* record_wnd = parent->get_record_gmp();
	LivePlayer* lr = record_wnd->get_live_player();
	LivePlayer* lp = parent->get_live_player();
	//if ((live_player!=NULL)&&(live_player == lr)) { // 录制的时候再录制跳出警告并退出
	if ((NULL != lr)) { // 录制的时候再录制跳出警告并退出
		Gtk::MessageDialog infoDialog(_("Already Recorded"),false,
				Gtk::MESSAGE_WARNING);
		Glib::ustring text_ = _("The Record had working now, you must stop it beforce");
		infoDialog.set_secondary_text(text_);
		infoDialog.run();
		return;
	}



	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return ;
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	Gtk::FileChooserDialog dlg(*parent,
			_("Choose File"), 
			Gtk::FILE_CHOOSER_ACTION_SAVE);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	if (Gtk::RESPONSE_OK == dlg.run()) {
		Glib::ustring outfilename = dlg.get_filename();
		if (outfilename.empty())
			return;

		//RecordStream* record_wnd = parent->get_record_gmp();
		record_wnd->set_out_file(outfilename);

		LivePlayer* live_player = get_player(stream, page);
		if (live_player == lp) { // 播放的时候
			// 停止播放
			parent->set_live_player(NULL);
			if (live_player->get_stream() == stream) {
				record_wnd->set_live_player(live_player, name);
				// 如果只是同一个频道在播放，直接转为录制.
				return;
			} else {
				delete live_player; 
				// 再重新生成一个
				live_player =  get_player(stream, page);
			}
		} else if (live_player == lr) { // 录制的时候
			if (live_player->get_stream() == stream) {
				// 是同一个频道在录制，怎么办？现在什么也不干
				// 。以后弹出个对话框警告一下
				return ;
				//parent->set_live_player(live_player, name);
			} else { // 停掉录制
				record_wnd->set_live_player(NULL);
				delete live_player;
				// 再重新生成一个
				live_player =  get_player(stream, page);
			}
		} else {
			delete lr;
		}
		record_wnd->set_live_player(NULL);
		record_wnd->set_live_player(live_player, name);
		RecentChannel* rc =
			dynamic_cast<RecentChannel*>(parent->get_recent_channel());
		if (this != rc)
			rc->saveLine(name,stream,page);
	}
}

std::string Channel::get_stream()
{
	Glib::RefPtr < Gtk::TreeSelection > selection =
		this->get_selection();
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (!selection->count_selected_rows())
		return std::string();
	return (*iter)[columns.stream];
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
	//printf("\nplay_selection_iter\n");
	TypeChannel page = (*iter)[columns.type];
	Glib::ustring name = (*iter)[columns.name];
	std::string stream = (*iter)[columns.stream];

	if(GROUP_CHANNEL == (*iter)[columns.type]) {
		parent->set_live_player(parent->get_live_player());
		return;
	}

	LivePlayer* live_player = get_player(stream, page);
	LivePlayer* lp = parent->get_live_player();
	LivePlayer* lr = parent->get_record_gmp()->get_live_player();

	if (live_player == lp) { // 播放的时候
		parent->set_live_player(NULL); // 停止播放
		if (live_player->get_stream() == stream) {
			// 如果只是播放同一个频道，就只是重启一下mplayer好了.
			parent->set_live_player(lp);
			return;
		} else {
			delete live_player;
			// 再重新生成一个
			LivePlayer* live_player = get_player(stream, page);
		}
	} else if (live_player == lr) { // 录制的时候
		Gtk::MessageDialog askDialog(_("Stop Recording"),
				false,
				Gtk::MESSAGE_QUESTION,
				Gtk::BUTTONS_OK_CANCEL);
		Glib::ustring text =  _("GMLive is recording,you can play channel after stop the record.\n Are you really to do this?");
		askDialog.set_secondary_text(text);
		if (Gtk::RESPONSE_OK != askDialog.run())
			return;

		parent->get_record_gmp()->set_live_player(NULL); // 停止录制吧
		if (live_player->get_stream() == stream) {
			// 是同一个频道，但是在录制，怎么办？停掉录制开始播放吧.
		}  else {
			delete live_player;
			// 再重新生成一个
			live_player = get_player(stream, page);
		}
	} else {
		delete lp;
	}

	parent->set_live_player(NULL);
	live_player = get_player(stream, page);

	parent->set_live_player(live_player, name);
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
bool Channel::on_leave_event(GdkEventCrossing * ev)
{
	if (tipTimeout.connected()) {
		tipTimeout.disconnect();
	}
	return false;
}

bool Channel::tooltip_timeout(GdkEventMotion * ev)
{
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn * column;
	int cell_x, cell_y;
	if (this->
			get_path_at_pos((int) ev->x, (int) ev->y, path, column, cell_x,
				cell_y)) {
		Gtk::TreeModel::iterator iter =
			this->get_model()->get_iter(path);
		if (!iter)
			return false;
		TypeChannel type = (*iter)[columns.type];
		Glib::ustring type_;
		if(PPLIVE_CHANNEL == type)
			type_  = _("PPLive Stream");
		if(SOPCAST_CHANNEL == type)
			type_ = _("SopCast Stream");
		else if(MMS_CHANNEL == type)
			type_ = _("MMS stream");
		Glib::ustring name = (*iter)[columns.name];
		int num = (*iter)[columns.users];
		std::stringstream ss;
		ss<<num;
		std::string user=ss.str();
		std::string stream = (*iter)[columns.stream];
		Glib::ustring text;

		if(PPLIVE_CHANNEL == type)
			text = "<span weight='bold'>" +name +"\n" + _("users:")+user+
			"\n<span weight='bold'></span>"+_("Type:")+type_+"\n</span>";
		else
			text = "<span weight='bold'>" +name +"\n" + _("users:")+user+
			"\nURL:</span> " + stream +"\n<span weight='bold'>"+_("Type:")+type_+"\n</span>";
		//Glib::RefPtr<Gdk::Pixbuf> logo= Gdk::Pixbuf::create_from_file(DATA_DIR"/gmlive.png");

		//tooltips->setImage(logo);
		tooltips->setLabel(text);
		tooltips->showTooltip(ev);

	}
	return false;
}
bool Channel::on_motion_event(GdkEventMotion * ev)
{
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn * column;
	int cell_x, cell_y;
	int delay = 600;

	if (tipTimeout.connected()) {

		tipTimeout.disconnect();
		tooltips->hideTooltip();
	}
	if (this->
			get_path_at_pos((int) ev->x, (int) ev->y, path, column, cell_x,
				cell_y)) {
		Gtk::TreeModel::iterator iter =
			this->get_model()->get_iter(path);
		TypeChannel type = (*iter)[columns.type];
		if (GROUP_CHANNEL != type)
			tipTimeout =
				Glib::signal_timeout().connect(sigc::bind <
						GdkEventMotion *
						>(sigc::
							mem_fun(*this,
								&Channel::
								tooltip_timeout),
							ev), delay);
		else
			tooltips->hideTooltip();
	} else
		tooltips->hideTooltip();

	return true;
}


bool Channel::on_tooltip_show(int x, int y, bool key_mode, const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn * column;
	int cell_x, cell_y;
	if (this->
			get_path_at_pos(x, y, path, column, cell_x,
				cell_y)) {
		Gtk::TreeModel::iterator iter =
			this->get_model()->get_iter(path);
		if (!iter){
			return false;
		}
		TypeChannel type = (*iter)[columns.type];
		Glib::ustring type_;
		if(PPLIVE_CHANNEL == type)
			type_  = _("PPLive Stream");
		else if(SOPCAST_CHANNEL == type)
			type_ = _("SopCast Stream");
		else if(MMS_CHANNEL == type)
			type_ = _("MMS stream");
		Glib::ustring name = (*iter)[columns.name];
		int num = (*iter)[columns.users];
		std::stringstream ss;
		ss<<num;
		std::string user=ss.str();
		std::string stream = (*iter)[columns.stream];
		Glib::ustring text;

		if(PPLIVE_CHANNEL == type){
			text = "<span weight='bold'>" +name +"\n" + _("users:")+user+
			"\n<span weight='bold'></span>"+_("Type:")+type_+"\n</span>";

		}
		else
			text = "<span weight='bold'>" +name +"\n" + _("users:")+user+
			"\nURL:</span> " + stream +"\n<span weight='bold'>"+_("Type:")+type_+"\n</span>";
		//Glib::RefPtr<Gdk::Pixbuf> logo= Gdk::Pixbuf::create_from_file(DATA_DIR"/gmlive.png");

		//tooltips->setImage(logo);
		tooltips->setLabel(text);
		//tooltips->showTooltip(ev);
		return true;

	}
	return false;
}

Glib::ustring Channel::user_select_list(const char* title)
{
	//属于读取默认列表不正确的处理
	std::string filename;
	Gtk::FileChooserDialog dialog(_("Please select a channel list file"),Gtk::FILE_CHOOSER_ACTION_OPEN);
	Gtk::MessageDialog askDialog(_("open channles error")
			,false
			,Gtk::MESSAGE_QUESTION
			,Gtk::BUTTONS_OK_CANCEL
			);
	askDialog.set_secondary_text(title);
	if (askDialog.run() == Gtk::RESPONSE_OK) {
		//open a file select window
		dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN,Gtk::RESPONSE_OK);
		if ( dialog.run() == Gtk::RESPONSE_OK)
			return dialog.get_filename();

	}
	return "";
}

