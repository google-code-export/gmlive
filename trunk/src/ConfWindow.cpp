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
	GlademmXML vbox_xml =
	    Gnome::Glade::Xml::create(main_ui, "vbox_conf");
	Gtk::VBox * vBox = dynamic_cast < Gtk::VBox * >
	    (vbox_xml->get_widget("vbox_conf"));

	/*
	check_embed = dynamic_cast<Gtk::CheckButton*>
		(vbox_xml->get_widget("check_embed"));
	mplayer_param = dynamic_cast<Gtk::Entry*>
		(vbox_xml->get_widget("entry_parameter"));
	mms_mplayer_cache = dynamic_cast<Gtk::Entry*>
		(vbox_xml->get_widget("entry_mms_cache"));
	sopcast_mplayer_cache = dynamic_cast<Gtk::Entry*>
		(vbox_xml->get_widget("entry_sopcast_cache"));
	nslive_mplayer_cache = dynamic_cast<Gtk::Entry*>
		(vbox_xml->get_widget("entry_nslive_cache"));
	nslive_delay_time = dynamic_cast<Gtk::Entry*>
		(vbox_xml->get_widget("entry_nslive_delay"));
		*/

	m_paramter=GMConf["mplayer_paramter"];
	m_mms_cache=GMConf["mms_mplayer_cache"];
	m_sopcast_cache = GMConf["sopcast_mplayer_cache"];
	m_nslive_cache = GMConf["nslive_mplayer_cache"];
	m_nslive_delay = GMConf["nslive_delay_time"];


	m_pVariablesMap = new Gnome::Glade::VariablesMap(vbox_xml);
	m_pVariablesMap->connect_widget("check_embed",m_embed);
	m_pVariablesMap->connect_widget("entry_parameter", m_paramter);
	m_pVariablesMap->connect_widget("entry_mms_cache", m_mms_cache);
	m_pVariablesMap->connect_widget("entry_nslive_cache",m_nslive_cache);
	m_pVariablesMap->connect_widget("entry_nslive_delay", m_nslive_delay);
	m_pVariablesMap->connect_widget("entry_sopcast_cache",m_sopcast_cache);



	vbox_xml->connect_clicked("button_save", sigc::mem_fun(*this,&ConfWindow::on_button_save));
	vbox_xml->connect_clicked("button_cancel", sigc::mem_fun(*this, &ConfWindow::on_button_cancel));

	add(*vBox);
	set_transient_for(*parent);


	set_default_size(600, 400);
	set_title(_("GMLive Conf Window"));
	show_all();
}

void ConfWindow::on_button_save()
{
	read_to_GMConf();
	save();
	delete this;
}


void ConfWindow::on_button_cancel()
{
	delete this;
}

void ConfWindow::read_to_GMConf()
{

	GMConf["mplayer_paramter"]      =            m_paramter   ; 
	GMConf["mms_mplayer_cache"]     =            m_mms_cache  ;
	GMConf["sopcast_mplayer_cache"] =            m_sopcast_cache;
	GMConf["nslive_mplayer_cache"]  =            m_nslive_cache ;
	GMConf["nslive_delay_time"]     =            m_nslive_delay ;



}

void ConfWindow::save()
{
		char buf[512];
		char* homedir = getenv("HOME");
		snprintf(buf, 512,"%s/.gmlive/config",homedir);
		std::ofstream file(buf);
		std::string line;
		std::map<std::string,std::string>::iterator iter=GMConf.begin();
		for(;iter != GMConf.end(); ++iter)
		{
			line = iter->first + "=" + iter->second;
			file<<line<<std::endl;
		}
		file.close();

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
