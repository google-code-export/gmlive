/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  网络电视图形外壳
 *
 *        Version:  1.0
 *        Created:  2007年11月25日 12时13分02秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <iostream>
#include "MainWindow.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using namespace std;
TGMConf GMConf;

void singleon()
{
	int sockfd,nbytes;
	char buf[1024];
	struct hostent *he;
	struct sockaddr_in srvaddr;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket error");
		exit(1);
	}
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(GMPORT);
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr*)&srvaddr,
			sizeof(struct sockaddr))==-1){
		printf("there has another gmlive running\n");
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	setlocale (LC_ALL, "");

	//保持只有一个gmlive实例
	singleon();

	bindtextdomain (GETTEXT_PACKAGE, GMLIVE_LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, NULL);
	textdomain (GETTEXT_PACKAGE);

	Gtk::Main kit(argc, argv);

	MainWindow wnd; 
	kit.run();

	return 0;
}

