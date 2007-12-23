/*
 * =====================================================================================
 *
 *       Filename:  recordStream.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月22日 20时15分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#define record_ui	    DATA_DIR"/record.xml"
#include "recordStream.h"
#include "gmlive.h"
#include "ec_throw.h"

RecordStream::RecordStream():
	live_player(NULL)
{
	GlademmXML ui_xml = Gnome::Glade::Xml::create(record_ui, "main_frame");
	if (!ui_xml) 
		exit(127);
	
	Gtk::VBox* main_frame = dynamic_cast < Gtk::VBox* >
		(ui_xml->get_widget("main_frame"));

	this->add(*main_frame);
	//this->show_all();

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/gmlive_default_out_file", homedir);
	outfilename.assign(buf);
	this->set_out_slot(sigc::mem_fun(*this, &RecordStream::on_mplayer_out));
}

RecordStream::~RecordStream()
{
}


void RecordStream::initialize()
{
	signal_start_.emit();
	char cache_buf[32];
	snprintf(cache_buf, 32, "%d", cache);
	const char* argv[10];
	argv[0] = "mplayer";
	argv[1] = "-slave";
	argv[2] = "-idle";
	argv[3] = "-cache";
	argv[4] = cache_buf;
	argv[5] = "-dumpstream";
	argv[6] = "-dumpfile";
	argv[7] = outfilename.c_str();
	argv[8] = "-quiet";
	argv[9] = NULL;

	my_system((char* const *) argv);
}

void RecordStream::start(const std::string& filename_)
{
	if (filename != filename_)
		filename = filename_;
	start();
}

void RecordStream::start()
{
	if (running())
		stop();
	initialize();
	char cb[256];
	int len = snprintf(cb, 256, "loadfile %s\n", filename.c_str());
	EC_THROW(-1 == write(stdin_pipe[1], cb, len));
}

void RecordStream::set_out_file(const std::string& filename)
{
	outfilename = filename;
}

void RecordStream::on_mplayer_exit()
{
}

bool RecordStream::on_mplayer_out(const Glib::IOCondition& condition)
{

}

void RecordStream::set_live_player(LivePlayer* lp,
	       	const std::string& name)
{
	std::cout << name << std::endl;
	if (lp != NULL) {
		record_channel_name = name;
		live_player = lp;
	//	lp->signal_status().connect(sigc::mem_fun(
	//				*this, &RecordStream::on_live_player_out));
		lp->start(*this);
	} else if (live_player) {
		live_player->start(*this);
	}	
}

