/*
 * =====================================================================================
 * 
 *       Filename:  pst_ctrl.h
 * 
 *    Description:  伪终端控制
 * 
 *        Version:  1.0
 *        Created:  2007年11月28日 19时28分26秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */

#ifndef _PST_CTRL_H_
#define _PST_CTRL_H_

#include <stdlib.h>
#include <fcntl.h>
#include "ec_throw.h"
#include <string>

class PstCtrl {
	public:
		PstCtrl();
		~PstCtrl();
		void setup_slave();
		void wait_slave();
		int get_ptm() const { return ptm; }
		void close_ptm() { ::close(ptm); }
	private:
		std::string ptsname;
		int ptm;
		int pts;
};

#endif // _PST_CTRL_H_

