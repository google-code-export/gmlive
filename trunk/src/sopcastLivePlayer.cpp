#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include "sopcastLivePlayer.h"
#include "gmlive.h"
#include "ec_throw.h"
#include "scope_gruard.h"
#include <gmplayer.h>

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


SopcastLivePlayer::SopcastLivePlayer(const std::string& stream_) : 
	stream(stream_),
	sop_pid(-1),
	sop_sock(-1),
	player(false),
	gmplayer(NULL)
{
}

SopcastLivePlayer::~SopcastLivePlayer()
{
	stop();
	close(sop_sock);
	printf("sopcast exit\n");
}

void SopcastLivePlayer::play(GMplayer& gmp)
{
	gmplayer = &gmp;
	extern char **environ;
	int pid = fork();
	if (pid == -1)
		return ;
	if (pid == 0) {
		close(STDOUT_FILENO);

		const char* argv[5];

		argv[0] = "sp-sc-auth";
		argv[1] = stream.c_str();
		argv[2] = "3908";
		argv[3] = "8908";
		argv[4] = NULL;

		// 设置 这个子进程为进程组头，
		// 这样，只要杀掉这个进程，他的子进程也会退出
		EC_THROW(-1 == setpgid(0, 0));
		execvp(argv[0], (char* const *)argv);
		perror("sp-sc-auth execvp:");
		exit(127);
	} 

	sop_pid = pid;
	printf("sop_pid = %d\n",sop_pid);

	signal_status_.emit(0);

	sop_time_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SopcastLivePlayer::on_sop_time_status), 1000);

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

	int i = atoi(buf);
	if ((i > 70) && (!player)){
		player = true;
		std::string& cache = GMConf["sopcast_mplayer_cache"];
		int icache = atoi(cache.c_str());
		icache = icache > 64 ? icache : 64;

		gmplayer->set_cache(icache);
		gmplayer->play(SOPCASTSTREAM);

		sop_time_conn.disconnect(); // 启动mpaleyr，停掉显示缓冲状态
		close(sop_sock);	   // 关掉状态查询端口
		return false;
	}
	signal_status_.emit(i);
	return true;

}

void SopcastLivePlayer::stop()
{
	if (sop_pid > 0) {
		player = false;
		sop_time_conn.disconnect();
		sop_sock_conn.disconnect();
		
		for (;;) {
			kill(-sop_pid, SIGKILL);
			int ret = (waitpid( -sop_pid, NULL, WNOHANG));
			if (-1 == ret)
				break;
		}
		sop_pid = -1;
	}

}

