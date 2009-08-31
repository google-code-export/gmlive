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
#include "ec_throw.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using namespace std;
TGMConf GMConf;

//检测单一实例并发送网址
int singleon(const std::string& url)
{
	int sockfd,nbytes;
	char buf[1024];
	struct hostent *he;
	struct sockaddr_in srvaddr;

	EC_THROW(-1 == (sockfd=socket(AF_INET,SOCK_STREAM,0)));
	int on = 1;
	EC_THROW( -1 == (setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) )));
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(GMPORT);
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	EC_THROW( -1 == (setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) )));
	if(bind(sockfd,(struct sockaddr*)&srvaddr,
			sizeof(struct sockaddr))==-1){
		printf("there has another gmlive running\n");
		if(url.empty())
			exit(0);
		else{
			//把网址发送给另一个gmlive再退出
			std::cout<<"send url to "<<url<<std::endl;
			if( 0 == connect(sockfd,(struct sockaddr*)&srvaddr,sizeof(srvaddr))){
				write(sockfd,url.c_str(),url.size());
				close(sockfd);
				exit(0);

			}
			exit(1);
		}
	}
	EC_THROW(-1 == listen(sockfd,128));
	return sockfd;
}

int main(int argc, char* argv[])
{
	setlocale (LC_ALL, "");

	std::string url;

	if(2==argc){
		url = std::string(argv[1]);
	}
	else{
		url=std::string("");

	}
	//保持只有一个gmlive实例
	int fd_io = singleon(url);

	bindtextdomain (GETTEXT_PACKAGE, GMLIVE_LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, NULL);
	textdomain (GETTEXT_PACKAGE);

	Gtk::Main kit(argc, argv);

	MainWindow wnd; 
	wnd.watch_socket(fd_io);
		
	kit.run();

	return 0;
}

