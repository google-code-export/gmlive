#ifndef GMLIVE_H_
#define GMLIVE_H_

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <glib/gi18n.h>
#include <iostream>

#define PAGE_PICTURE 0
#define PAGE_MPLAYER 1
#define NSLIVESTREAM "http://127.0.0.1:9000"
#define SOPCASTSTREAM "http://127.0.0.1:8908/tv.asf"

#define main_ui	    DATA_DIR"/gmlive.xml"

typedef Glib::RefPtr < Gnome::Glade::Xml > GlademmXML;

enum TypeChannel{
	NSLIVE_CHANNEL,
	MMS_CHANNEL,
	SOPCAST_CHANNEL,
	GROUP_CHANNEL,
	NONE
};

typedef struct{
	bool embed;
}Setting;

#endif
