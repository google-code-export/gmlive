#ifndef GMLIVE_H_
#define GMLIVE_H_

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <glib/gi18n.h>
#include <iostream>
#include <map>

#define PAGE_PICTURE 0
#define PAGE_MPLAYER 1
#define PPLIVESTREAM "http://127.0.0.1:1024/1.asf"
#define SOPCASTSTREAM "http://127.0.0.1:8908/tv.asf"


#define main_ui	    DATA_DIR"/gmlive.glade"
typedef Glib::RefPtr < Gnome::Glade::Xml > GlademmXML;

enum TypeChannel{
	PPLIVE_CHANNEL,
	MMS_CHANNEL,
	SOPCAST_CHANNEL,
	GROUP_CHANNEL,
	NONE
};


typedef std::map<std::string,std::string> TGMConf;
extern TGMConf GMConf;

#ifndef DEBUG
#define DLOG(fmt, ...) \
    { \
        fprintf(stderr, "%s|%d| " fmt, \
                 __FILE__, __LINE__, ##__VA_ARGS__); \
    }
#else
#define DLOG(fmt, ...) \
	;
#endif




#endif
