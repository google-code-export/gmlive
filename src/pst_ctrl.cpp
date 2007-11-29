/*
 * =====================================================================================
 *
 *       Filename:  pst_ctrl.cpp
 *
 *    Description:  伪终端控制
 *
 *        Version:  1.0
 *        Created:  2007年11月28日 19时41分19秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 *
 * =====================================================================================
 */
#include "pst_ctrl.h"
#include <sys/select.h>

PstCtrl::PstCtrl() :
	pts (-1)
{
	// 打开伪终端的主则.
	EC_THROW( -1 == ( ptm = posix_openpt( O_RDWR | O_NOCTTY )));
	
	// 取得从则访问权限
	EC_THROW( -1 == ( grantpt(ptm)));

	// 解锁 pty
	EC_THROW( -1 == ( unlockpt(ptm)));

	// 取得pty从则的名字
	char buf[256];
	EC_THROW( 0 != (ptsname_r(ptm, buf, 256)));
	ptsname.assign(buf);
}

PstCtrl::~PstCtrl()
{
}

void PstCtrl::setup_slave()
{
	EC_THROW( -1 == setsid());

	if (pts != -1)
		EC_THROW(-1 == close(pts));

	fprintf(stderr, "%s\n", ptsname.c_str());
	EC_THROW( -1 == (pts = open(ptsname.c_str(), O_RDWR)));

	EC_THROW(fchmod(pts, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) == -1 && errno != EPERM) ;

	EC_THROW( -1 == dup2(pts, STDIN_FILENO));
	EC_THROW( -1 == dup2(pts, STDOUT_FILENO));
	EC_THROW( -1 == dup2(pts, STDERR_FILENO));
}


void PstCtrl::wait_slave()
{
	fd_set fd_set_write;

	FD_ZERO(&fd_set_write);
	FD_SET(ptm, &fd_set_write);

	EC_THROW( -1 == select (ptm + 1, NULL, &fd_set_write, NULL, NULL));

}


