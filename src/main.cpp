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

#include <iostream>
#include "MainWindow.h"

using namespace std;
TGMConf GMConf;

int main(int argc, char* argv[])
{
	Gtk::Main kit(argc, argv);

	MainWindow wnd; 
	kit.run(wnd);

	return 0;
}

