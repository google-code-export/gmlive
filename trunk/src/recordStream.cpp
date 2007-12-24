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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

RecordStream::RecordStream():
	live_player(NULL)
	,outfile(-1)
{
	GlademmXML ui_xml = Gnome::Glade::Xml::create(record_ui, "main_frame");
	if (!ui_xml) 
		exit(127);

	Gtk::VBox* main_frame = dynamic_cast < Gtk::VBox* >
		(ui_xml->get_widget("main_frame"));

	this->add(*main_frame);

	record_name = dynamic_cast <Gtk::Label*>
		(ui_xml->get_widget("label_info"));

	progress_bar = dynamic_cast<Gtk::ProgressBar*>
		(ui_xml->get_widget("progressbar"));

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/gmlive_default_out_file", homedir);
	outfilename.assign(buf);
	this->set_out_slot(sigc::mem_fun(*this, &RecordStream::on_mplayer_out));

	ui_xml->connect_clicked("bt_preview", sigc::mem_fun(*this, &RecordStream::on_preview));
	ui_xml->connect_clicked("bt_stop", sigc::mem_fun(*this, &RecordStream::on_stop));
	//this->show_all();
}

RecordStream::~RecordStream()
{
	stop();
}

void RecordStream::on_preview()
{
	std::cout << "on_preview" << std::endl;
}

void RecordStream::on_stop()
{
	stop();
	std::cout << "on_stop" << std::endl;

	Gtk::MessageDialog infoDialog(_("Record finial"));
	Glib::ustring text = record_channel_name + _(" record finial");
	infoDialog.set_secondary_text(text);
	infoDialog.run();
	record_name->set_label("");

	//delete live_player;
	//live_player = NULL;
	delete this;

}

bool RecordStream::on_delete_event(GdkEventAny* event)
{
	stop();
	delete live_player;
	live_player = NULL;
	return Gtk::Window::on_delete_event(event);
}

void RecordStream::initialize()
{
	signal_start().emit();
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
	//argv[8] = "-quiet";
	argv[8] = NULL;

	if (-1 != my_system((char* const *) argv) )
		timeout_conn = Glib::signal_timeout().connect(
				sigc::mem_fun(*this, &RecordStream::on_timeout), 2000);
}

void RecordStream::start(const std::string& filename_)
{
	if (filename != filename_)
		filename = filename_;

	Glib::ustring info = filename + " >> " + outfilename;
	record_name->set_label(info);
	start();
	show_all();
}

void RecordStream::start()
{
	if (running())
		stop();
	initialize();
	char cb[256];
	int len = snprintf(cb, 256, "loadfile %s\n", filename.c_str());
	send_ctrl_command(cb);
}


void RecordStream::set_out_file(const std::string& filename)
{
	outfilename = filename;
}

void RecordStream::on_mplayer_exit()
{
	signal_stop().emit();
//	delete live_player;
	timeout_conn.disconnect();
	close(outfile);
	outfile = -1;
}

bool RecordStream::on_timeout()
{
	progress_bar->pulse();

	if (-1 == outfile)
		outfile = open(outfilename.c_str(), O_RDONLY);
	if (-1 == outfile)
		return true;

	struct stat stat;
	EC_THROW(-1 == fstat(outfile, &stat));
	off_t size = stat.st_size/(1024);
	char buf[512];
	int len = snprintf(buf, 512, "Recording %s To %s (%u Kb)",record_channel_name.c_str(), outfilename.c_str(), size);
	buf[len] = 0;
	record_name->set_label(buf);
	return true;
}

bool RecordStream::on_mplayer_out(const Glib::IOCondition& condition)
{
	char buf[256];
	int len = this->get_mplayer_log(buf, 255);
	buf[len] = 0;
	printf("%s", buf);
}

void RecordStream::on_live_player_out(int percentage)
{
}

void RecordStream::set_live_player(LivePlayer* lp,
		const std::string& name)
{
	this->show_all();
	if (lp != NULL) {
		if (!name.empty())
			record_channel_name = name;
		if (live_player != lp) {
			delete live_player;
			live_player = lp;
			lp->signal_status().connect(sigc::mem_fun(
						*this, &RecordStream::on_live_player_out));
		}
		lp->start(*this);
	} else if (live_player) {
		live_player = NULL;
		this->stop();
		this->hide();
	}	
}

