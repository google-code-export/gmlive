#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include "sopcast_live_player.h"
#include "gmlive.h"
#include "ec_throw.h"
#include "scope_gruard.h"

int connect_to_server(const char *host, int portnum)
{
	int sock;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	EC_THROW(-1 == (sock = socket(AF_INET, SOCK_STREAM, 0)));
	ScopeGuard sg = MakeGuard(&close, sock);
	EC_THROW(NULL == (server = gethostbyname(host)));

	memset((char *) &serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	memcpy((char *) &serv_addr.sin_addr.s_addr,
			(char *) server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portnum);

	EC_THROW( -1 == connect(sock, 
				(struct sockaddr *) &serv_addr,
			       	sizeof(serv_addr)));
	sg.dismiss();
	return sock;

}


SopcastLivePlayer::SopcastLivePlayer(GMplayer& gmp, const std::string& stream_) : 
	LivePlayer(gmp),
	stream(stream_),
	sop_pid(-1),
	sop_sock(-1),
	player(false)
{
}

SopcastLivePlayer::~SopcastLivePlayer()
{
	stop();
	close(sop_sock);
	printf("sopcast exit\n");
}

void SopcastLivePlayer::play()
{
	extern char **environ;
	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);

		char buf[256];
		//sprintf(buf, "sp-sc-auth %s 3908 8908 ", stream.c_str());
		const char* argv[5];
	//	argv[0] = "sh";
	//	argv[1] = "-c";
	//	argv[2] = buf;
	//	argv[3] = NULL;
		argv[0] = "sp-sc-auth";
		argv[1] = stream.c_str();
		argv[2] = "3908";
		argv[3] = "8908";
		argv[4] = NULL;

		//const char* argv[4] = {"sh", "-c", buf};
		//execve("/bin/sh", (char**)argv, environ);
		execvp(argv[0], (char* const *)argv);
		perror("sp-sc-auth execvp:");
		exit(127);
	} 

	sop_pid = pid;
	printf("%d\n",sop_pid);

	signal_status_.emit(0);

	sop_time_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SopcastLivePlayer::on_sop_time_status), 1000);

	//gmp.start(SOPCASTSTREAM);
}

bool SopcastLivePlayer::on_sop_time_status()
{
	if (-1 == sop_sock) {
		try {
			sop_sock = connect_to_server("127.0.0.1", 8908);
			for (int i = 0; i < 2; i++)
				EC_THROW( -1 == write(sop_sock, "state\ns\n", sizeof("state\ns\n")));

			sop_sock_conn = Glib::signal_io().connect(
					sigc::mem_fun(*this, &SopcastLivePlayer::on_sop_sock),
					sop_sock, Glib::IO_IN);	
		} catch (...)
		{}
	}

   	write(sop_sock, "s\n", sizeof("s\n"));
	return true;
}

bool SopcastLivePlayer::on_sop_sock(const Glib::IOCondition& condition)
{
	int byte_read = 0;
	int buf_size = 255;
	char buf[buf_size + 1] ;
	char *pstr = NULL;
	memset(buf, 0, sizeof(buf));

	while (byte_read < buf_size) {
		byte_read += read(sop_sock, buf + byte_read, buf_size - byte_read);
		if ((pstr = strstr(buf, "\n")) != 0) {
			pstr++;
			*pstr = 0;
			break;
		}
	}

	//printf("%s", buf);
	int i = atoi(buf);
	if ((i > 70) && (!player)){
		player = true;
		gmp.start(SOPCASTSTREAM);

		sop_time_conn.disconnect(); // 启动mpaleyr，停掉显示缓冲状态
		return false;
	}
	signal_status_.emit(i);
	return true;

}

void SopcastLivePlayer::stop()
{
	gmp.stop();
	if (sop_pid > 0) {
		player = false;
		sop_time_conn.disconnect();
		sop_sock_conn.disconnect();
		kill(sop_pid, SIGKILL);
		//kill(sop_pid+1, SIGKILL);
		waitpid(sop_pid, NULL, 0);
		sop_pid = -1;
	}

}

