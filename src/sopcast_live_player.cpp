#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "sopcast_live_player.h"
#include "gmlive.h"


SopcastLivePlayer::SopcastLivePlayer(GMplayer& gmp, const std::string& stream_) : 
	LivePlayer(gmp),
	stream(stream_),
	sop_pid(-1)
{
}

SopcastLivePlayer::~SopcastLivePlayer()
{
	stop();
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
		sprintf(buf, "sp-sc-auth %s 3908 8908 >/dev/null", stream.c_str());
		const char* argv[5];
		argv[0] = "sp-sc-auth";
		argv[1] = stream.c_str();
		argv[2] = "3908";
		argv[3] = "8908";
		argv[4] = NULL;

		//const char* argv[4] = {"sh", "-c", buf};
		//execve("/bin/sh", (char**)argv, environ);
		execvp("sp-sc-auth", (char* const *)argv);
		perror("sp-sc-auth execvp:");
		exit(127);
	} 

	sop_pid = pid;
	printf("%d\n",sop_pid);
	gmp.start(SOPCASTSTREAM);
}

void SopcastLivePlayer::stop()
{
	gmp.stop();
	if (sop_pid > 0) {
		kill(sop_pid, SIGKILL);
		//kill(sop_pid+1, SIGKILL);
		waitpid(sop_pid, NULL, 0);
		sop_pid = -1;
	}

}

