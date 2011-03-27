/*
 * =====================================================================================
 *
 *       Filename:  ConfWindow.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月17日 21时49分13秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */

#include "ConfWindow.h"
#include "MainWindow.h"
#include <fstream>

ConfWindow::ConfWindow(MainWindow * parent_):parent(parent_)
{
	BuilderXML vbox_xml =
		Gtk::Builder::create_from_file(main_ui, "vbox_conf");
	Gtk::VBox * vBox = 0;
	vbox_xml->get_widget("vbox_conf", vBox);

	Gtk::Notebook* notebook = 0;
	vbox_xml->get_widget("notebook_conf", notebook);

	std::string& oplayer = GMConf["player_type"];
	m_oplayer = (!oplayer.empty())&&(oplayer[0]=='1');
	m_oplayer_cmd = GMConf["other_player_cmd"];

	std::string& embed = GMConf["mplayer_embed"];
	m_embed = (!embed.empty()) && (embed[0] == '1');
	std::string& enable = GMConf["enable_pplive"];
	m_enable_pplive = (!enable.empty())&&(enable[0] == '1' );
	enable = GMConf["enable_pps"];
	m_enable_pps = (!enable.empty())&&(enable[0] == '1' );
	enable = GMConf["enable_sopcast"];
	m_enable_sopcast = (!enable.empty())&&(enable[0] == '1' );

	std::string& check_sop_list = GMConf["check_refresh_sopcast_channels"];
	m_check_refresh_sopcast_channels = (!check_sop_list.empty())&&(check_sop_list[0] == '1');

	std::string& check_pplive_list = GMConf["check_refresh_pplive_channels"];
	m_check_refresh_pplive_channels = (!check_pplive_list.empty())&&(check_pplive_list[0] == '1');

	std::string& close_try = GMConf["close_to_systray"];
	m_close_try = (!close_try.empty()) && (close_try[0] == '1');

	std::string& enable_try =GMConf["enable_tray"];
	m_enable_try = (!enable_try.empty())&&(enable_try[0]=='1');

	m_paramter=GMConf["mplayer_paramter"];
	m_mms_cache=GMConf["mms_mplayer_cache"];
	m_sopcast_cache = GMConf["sopcast_mplayer_cache"];
	m_pplive_cache = GMConf["pplive_mplayer_cache"];
	m_pplive_delay = GMConf["pplive_delay_time"];
	m_sopcast_channel = GMConf["sopcast_channel_url"];
	m_mms_channel   =   GMConf["mms_channel_url"];
	m_pplive_channel =  GMConf["pplive_channel_url"];


	m_pVariablesMap = new VariablesMap(vbox_xml);
	m_pVariablesMap->connect_widget("rbtn_oplayer",m_oplayer);
	m_pVariablesMap->connect_widget("entry_oplayer_cmd",m_oplayer_cmd);
	m_pVariablesMap->connect_widget("check_embed",m_embed);
	m_pVariablesMap->connect_widget("enable_sopcast", m_enable_sopcast);
	m_pVariablesMap->connect_widget("enable_pplive", m_enable_pplive);
	m_pVariablesMap->connect_widget("enable_pps", m_enable_pps);
	m_pVariablesMap->connect_widget("entry_parameter", m_paramter);
	m_pVariablesMap->connect_widget("entry_mms_cache", m_mms_cache);

	if(m_enable_pplive)
	{
		m_pVariablesMap->connect_widget("entry_pplive_cache",m_pplive_cache);
		m_pVariablesMap->connect_widget("entry_pplive_delay", m_pplive_delay);
		m_pVariablesMap->connect_widget("entry_pplive_channel",m_pplive_channel);
		m_pVariablesMap->connect_widget("check_refresh_pplive_channels",m_check_refresh_pplive_channels);
	}
	if(m_enable_sopcast)
	{
		m_pVariablesMap->connect_widget("entry_sopcast_cache",m_sopcast_cache);
		m_pVariablesMap->connect_widget("entry_sopcast_channel",m_sopcast_channel);
		m_pVariablesMap->connect_widget("check_refresh_sopcast_channels",m_check_refresh_sopcast_channels);
	}
	m_pVariablesMap->connect_widget("entry_mms_channel",m_mms_channel);
	m_pVariablesMap->connect_widget("check_close_try",m_close_try);
	m_pVariablesMap->connect_widget("enable_tray",m_enable_try);



	Gtk::Button* bt = 0;
	vbox_xml->get_widget("button_save", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this,&ConfWindow::on_button_save));

	bt = 0;
	vbox_xml->get_widget("button_cancel", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this,&ConfWindow::on_button_cancel));


	enable_tray = 0;
	vbox_xml->get_widget("enable_tray", enable_tray);

	check_close_tray = 	0;
	vbox_xml->get_widget("check_close_try",check_close_tray);

	enable_tray->signal_clicked().connect(sigc::mem_fun(*this, &ConfWindow::on_enable_tray_toggle));
	on_enable_tray_toggle();

	radio_player = 0;
	vbox_xml->get_widget("rbtn_mplayer", radio_player);
	hbox2 = 0;
	vbox_xml->get_widget("hbox2", hbox2);
	radio_player->signal_clicked().connect(sigc::mem_fun(*this, &ConfWindow::on_select_mplayer));
	on_select_mplayer();

	m_pVariablesMap->transfer_variables_to_widgets();
	add(*vBox);
	set_transient_for(*parent);

	if(!m_enable_sopcast)
		notebook->remove_page(3);
	//if(!m_enable_pplive)
		notebook->remove_page(2);

	set_default_size(600, 400);
	set_title(_("GMLive Conf Window"));
	show_all();
}

ConfWindow::~ConfWindow()
{
	delete m_pVariablesMap;
}

void ConfWindow::on_button_save()
{
	write_to_GMConf();
	signal_quit_.emit();
	on_button_cancel();
}


void ConfWindow::on_button_cancel()
{
	//delete this;
	parent->on_conf_window_close(this);
}

void ConfWindow::write_to_GMConf()
{
	m_pVariablesMap->transfer_widgets_to_variables();

	GMConf["player_type"] = m_oplayer ? "1" : "0";
	GMConf["mplayer_embed"] = m_embed ? "1" : "0";
	GMConf["enable_pplive"] = m_enable_pplive ? "1" : "0";
	GMConf["enable_pps"] = m_enable_pps ? "1" : "0" ;
	GMConf["enable_sopcast"] = m_enable_sopcast ? "1" : "0";
	GMConf["mplayer_paramter"]      =            m_paramter   ; 
	GMConf["mms_mplayer_cache"]     =            m_mms_cache  ;
	GMConf["sopcast_mplayer_cache"] =            m_sopcast_cache;
	GMConf["pplive_mplayer_cache"]  =            m_pplive_cache ;
	GMConf["pplive_delay_time"]     =            m_pplive_delay ;
	GMConf["pplive_channel_url"]    =	     m_pplive_channel;
	GMConf["sopcast_channel_url"]   =            m_sopcast_channel;
	GMConf["mms_channel_url"]       =            m_mms_channel; 
	GMConf["other_player_cmd"]  = m_oplayer_cmd;
	GMConf["check_refresh_sopcast_channels"] = m_check_refresh_sopcast_channels ? "1" : "0";
	GMConf["check_refresh_pplive_channels"] = m_check_refresh_pplive_channels ? "1" : "0";
	GMConf["close_to_systray"] = m_close_try ? "1" : "0";
	GMConf["enable_tray"] = m_enable_try?"1":"0";
}


bool ConfWindow::on_key_press_event(GdkEventKey * ev)
{
	if (ev->type != GDK_KEY_PRESS)
		return Gtk::Window::on_key_press_event(ev);
	switch (ev->keyval) {
		case GDK_Escape:
			on_button_cancel();
			break;
		default:
			return Gtk::Window::on_key_press_event(ev);
	}
	return true;
}

bool ConfWindow::on_delete_event(GdkEventAny* ev)
{
	on_button_cancel();
	return true;
}

void ConfWindow::on_enable_tray_toggle()
{
	check_close_tray->set_sensitive(enable_tray->get_active());
}

void ConfWindow::on_select_mplayer()
{

	hbox2->set_sensitive(!radio_player->get_active());

}
