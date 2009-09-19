#include "gmplayer.h"
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "ec_throw.h"
#include <cassert>
#include "gmlive.h"
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

void  set_signals()
{
	sigset_t set;
	struct sigaction act;

	EC_THROW( -1 == sigfillset(&set));
	EC_THROW( -1 == sigprocmask(SIG_SETMASK, &set, NULL));

	memset(&act, 0, sizeof(act));

	EC_THROW( sigfillset(&act.sa_mask));
	act.sa_handler = SIG_IGN;
	EC_THROW( sigaction(SIGHUP, &act, NULL));
	EC_THROW( sigaction(SIGINT, &act, NULL));
	EC_THROW( sigaction(SIGQUIT, &act, NULL));
	EC_THROW( sigaction(SIGPIPE, &act, NULL));

	EC_THROW( sigaction(SIGTERM, &act, NULL));
	EC_THROW( sigaction(SIGBUS, &act, NULL));
	EC_THROW( sigaction(SIGFPE, &act, NULL));
	EC_THROW( sigaction(SIGILL, &act, NULL));
	EC_THROW( sigaction(SIGSEGV, &act, NULL));
	EC_THROW( sigaction(SIGSYS, &act, NULL));
	EC_THROW( sigaction(SIGXCPU, &act, NULL));
	EC_THROW( sigaction(SIGXFSZ, &act, NULL));
	EC_THROW( sigaction(SIGHUP, &act, NULL));

	EC_THROW( sigemptyset(&set));
	EC_THROW( sigprocmask(SIG_SETMASK, &set, NULL));

}

void GMplayer::create_pipe()
{
	close_pipe();

	int std_pipe[2];
	EC_THROW(-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, std_pipe));
	player_child_tem = std_pipe[0];
	player_child_tem2 = std_pipe[1];
}

void GMplayer::close_pipe()
{
	if ( -1 != player_child_tem)
		close(player_child_tem);
	if ( -1 != player_child_tem2)
		close(player_child_tem2);
}

void GMplayer::set_s_pipe()
{
	close(player_child_tem); // 关闭父进程端.
	EC_THROW( -1 == dup2(player_child_tem2, STDIN_FILENO));
	EC_THROW( -1 == dup2(player_child_tem2, STDOUT_FILENO));
	//EC_THROW( -1 == dup2(player_child_tem2, STDERR_FILENO));
	close(player_child_tem2);
}

void GMplayer::set_m_pipe()
{
	close(player_child_tem2);
	player_child_tem2 = -1;

	fd_set fd_set_write;
	FD_ZERO(&fd_set_write);
	FD_SET(player_child_tem, &fd_set_write);

	EC_THROW( -1 == select (player_child_tem + 1, NULL, &fd_set_write, NULL, NULL));

}


////////////////////////////////////////////////////////////////
#define record_ui	    DATA_DIR"/record.glade"
GMplayer::GMplayer() :
	is_running(false)
	,is_recording(false)
	,is_record(false)
	,is_pause(false)
	,player_pid(-1)
	,record_pid(-1)
	,player_child_tem(-1)
	,player_child_tem2(-1)
	,outfile(-1)
	,icache(64)
{
	BuilderXML ui_xml = Gtk::Builder::create_from_file(record_ui);
	if (!ui_xml) 
		exit(127);

	record_window = 0;
	ui_xml->get_widget("recordwindow", record_window);
	record_name = 0;
	ui_xml->get_widget("label_info", record_name);

	progress_bar = 0;
	ui_xml->get_widget("progressbar", progress_bar);

	char buf[512];
	char* homedir = getenv("HOME");
	snprintf(buf, 512,"%s/gmlive_default_out_file", homedir);
	outfilename.assign(buf);

	Gtk::Button* bt = 0;
	ui_xml->get_widget("bt_stop", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this, &GMplayer::on_stop_record));

	bt = 0;
	ui_xml->get_widget("bt_preview", bt);
	bt->signal_clicked().connect(sigc::mem_fun(*this, &GMplayer::on_preview));

	//ui_xml->connect_clicked("bt_stop", sigc::mem_fun(*this, &GMplayer::on_stop_record));
	//ui_xml->connect_clicked("bt_preview", sigc::mem_fun(*this, &GMplayer::on_preview));

	record_window->hide();
}

GMplayer::~GMplayer()
{
}

bool GMplayer::on_delay_reboot()
{
	if (is_recording)
		on_preview();
	else if (is_running)
		start_play();
	return false;
}

void GMplayer::wait_mplayer_exit(GPid pid, int)
{
	ptm_conn.disconnect();
	player_pid = -1;

	if (is_running) {
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &GMplayer::on_delay_reboot), 2000);
		return;
	}

	signal_stop().emit();

}

int GMplayer::my_system(char* const argv[])
{

	const char* p = argv[0];
	for (int i = 1 ; p ; p = argv[i++]   ){
		printf("%s ", p);
	}
	printf("\n");

	extern char **environ;
	create_pipe();
	assert(!out_slot.empty());
	ptm_conn = Glib::signal_io().connect(
			out_slot,
			player_child_tem, Glib::IO_IN);


	pid_t pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0) {

		set_signals();
		set_s_pipe();
		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		//sleep(4);
		execvp(argv[0], argv);

		perror("player mplayer execvp:");
	} 

	set_m_pipe();


	wait_conn = Glib::signal_child_watch().connect(
			sigc::mem_fun(*this, &GMplayer::wait_mplayer_exit), pid, 0);

	is_running = true;
	return pid;
}


void GMplayer::send_ctrl_command(const char* c)
{
	if (running()) {
		assert(c != NULL);
		EC_THROW(-1 == write(player_child_tem, c, strlen (c)));
	}
}

void GMplayer::pause()
{
	is_pause = !is_pause;
	send_ctrl_command("pause\n");
}

void GMplayer::mute(bool mute)
{
	if(mute)
		send_ctrl_command("mute\n");
	else
		send_ctrl_command("unmute\n");
}

void GMplayer::start_play()
{
	DLOG("\n");
	if (pausing())
		return pause();
	if (player_pid != -1)
		stop();

	signal_start().emit();

	std::list<std::string> pars;
	std::list<std::string>::iterator iter;
	if(	GMConf["player_type"] == "1"){  //other player handle
		std::string& oplayer_cmd = GMConf["other_player_cmd"];
		if(!oplayer_cmd.empty()){
			size_t pose1 = 0;
			size_t pose2 = 0;
			for(;;) {
				pose2 = oplayer_cmd.find_first_of(" \t", pose1);
				pars.push_back(oplayer_cmd.substr(pose1, pose2 - pose1));
				if (pose2 == std::string::npos)
					break;
				pose1 = pose2 + 1;
			}
		}
		else
		{
			printf("%s:%d\n",__func__,__LINE__);
			//handle here
			return;
		}

		int argv_len = 512 + pars.size();
		printf("%s:%d :argv_len = %d\n",__func__,__LINE__,argv_len);
		const char* argv[argv_len];
		int i = 0;
		iter = pars.begin();
		for (; i < argv_len && iter != pars.end(); i++, ++iter) {
			argv[i] = (*iter).c_str();
			printf("argv[%d]=%s\n",i,argv[i]);
		}

		argv[i] = playfilename.c_str();
		i++;
		argv[i] = NULL;

		player_pid = my_system((char* const *) argv);

	}
	else{  //mplayer handle


		std::string& paramter = GMConf["mplayer_paramter"];
		if (!paramter.empty()) {
			size_t pos1 = 0;
			size_t pos2 = 0;
			for(;;) {
				pos2 = paramter.find_first_of(" \t", pos1);
				pars.push_back(paramter.substr(pos1, pos2 - pos1));
				if (pos2 == std::string::npos)
					break;
				pos1 = pos2 + 1;
			}
		}

		int argv_len = 12 + pars.size();
		const char* argv[argv_len];
		argv[0] = "mplayer";
		argv[1] = "-slave";
		argv[2] = "-quiet";
		argv[3] = "-cache";
		char cache_buf[32];
		sprintf(cache_buf, "%d", icache);
		argv[4] = cache_buf;
		iter = pars.begin();
		int i = 5;
		for (; i < argv_len && iter != pars.end(); i++, ++iter) {
			argv[i] = (*iter).c_str();
		}
		if (GMConf["mplayer_embed"] == "1"){ 
			char wid_buf[32];
			snprintf(wid_buf, 32, "%d", this->get_id());
			printf("%s\n", wid_buf);
			argv[i++] = "-nomouseinput";
			argv[i++] = "-noconsolecontrols";
			argv[i++] = "-wid";
			argv[i++] = wid_buf;
		}
		argv[i++] = playfilename.c_str();
		argv[i] = NULL;


		player_pid = my_system((char* const *) argv);
		if (is_recording && record_pid == -1)
			on_start_record();
	}
}

void GMplayer::on_preview()
{
	if (0 == access(outfilename.c_str(), F_OK)) {
		start_play(outfilename);
	}
}

void GMplayer::start_record()
{
	DLOG("\n");
	if (running()) {
		if (is_recording)
			return;
		else {
			stop();
			is_record = true;
			is_recording = true;
			on_start_record();
			return;
		}
	} else {
		is_recording = true;
		on_start_record();
	}
}

void GMplayer::on_start_record()
{
	if (recordfilename.empty()) //  从播放直接转为录制时， recordfilename 是空的
		recordfilename = playfilename;
	DLOG("");
	const char* argv[10];
	argv[0] = "mplayer";
	argv[1] = "-slave";
	argv[2] = "-cache";
	char cache_buf[32];
	sprintf(cache_buf, "%d", icache);
	argv[3] = cache_buf;
	argv[4] = "-quiet";
	argv[5] = "-dumpstream";
	argv[6] = "-dumpfile";
	argv[7] = outfilename.c_str();
	argv[8] = recordfilename.c_str();
	argv[9] = NULL;


	pid_t pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		//sleep(4);
		execvp(argv[0], (char* const*)argv);

		perror("player mplayer execvp:");
	} 

	record_pid = pid;
	update_progress_conn = Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &GMplayer::on_update_progress), 2000);
	wait_record_conn = Glib::signal_child_watch().connect(
			sigc::mem_fun(*this, &GMplayer::wait_record_exit), pid, 0);

	record_window->show();
}

void GMplayer::start_record(const std::string& filename)
{
	if (filename != outfilename) {
		recordfilename = filename;
	}
	start_record();
}

void GMplayer::set_outfilename(const std::string& filename, const std::string& name)
{
	if (filename != outfilename) {
		outfilename = filename;
		std::string::size_type pos = filename.find_last_of('.');
		outfilenamebase = filename.substr(0, pos);
		if (pos != std::string::npos)
			outfilenameext = filename.substr(pos, std::string::npos);
		outfile_num = 0;
		channelname = name;
	}
}

void GMplayer::on_stop_record()
{
	is_recording = false;
	is_record = false;

	update_progress_conn.disconnect();
	wait_record_conn.disconnect();
	close(outfile);
	outfile = -1;
	std::cout << "on_stop" << std::endl;

	record_name->set_label("");
	record_window->hide();

	if (record_pid == -1)
		return;
	for (;;) {
		kill(-record_pid, SIGKILL);
		int ret = (waitpid( -record_pid, NULL, WNOHANG));
		if (-1 == ret)
			break;
	}
	record_pid = -1;

	Gtk::MessageDialog infoDialog(_("Record finial"));
	Glib::ustring text = channelname + _(" record finial");
	infoDialog.set_secondary_text(text);
	infoDialog.run();

}

bool GMplayer::on_delay_record_reboot()
{
	DLOG("\n");
	if (is_recording) {
		if (0 == access(outfilename.c_str(), F_OK)) {
			char buf[32];
			sprintf(buf, "-%d", outfile_num++);
			outfilename = outfilenamebase + buf + outfilenameext;
		}
		start_record();
	}
	return false;
}


void GMplayer::wait_record_exit(GPid pid, int)
{
	DLOG("\n");
	record_pid = -1;

	if (is_recording) {
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &GMplayer::on_delay_record_reboot), 2000);
		return;
	}
}

bool GMplayer::on_update_progress()
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
	int len;
	if(size<1024)
	{
		len = snprintf(buf, 512, _("Recording %s To %s (%u Kb)"),channelname.c_str(), outfilename.c_str(), size);
	}
	else{
		off_t mb=size/1024;
		off_t kb=size%1024;
		len = snprintf(buf, 512, _("Recording %s To %s (%u.%u Mb)"),channelname.c_str(), outfilename.c_str(), mb,kb);
	}
	buf[len] = 0;

	record_name->set_label(buf);
	return true;
}


void GMplayer::start_play(const std::string& filename)
{
	if (filename != playfilename)
		playfilename = filename;
	start_play();
}

void GMplayer::stop()
{
	is_running = false;
	is_recording = false;
	//is_record = false;
	if (player_pid != -1) {
		ptm_conn.disconnect();
		wait_conn.disconnect();

		for (;;) {
			kill(-player_pid, SIGKILL);
			int ret = (waitpid( -player_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}
		signal_stop().emit();
		player_pid = -1;	
	}

	if (record_pid != -1) {
		wait_record_conn.disconnect();
		on_stop_record();
	}
}

