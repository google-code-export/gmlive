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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using namespace std;
TGMConf GMConf;

int main(int argc, char* argv[])
{
	setlocale (LC_ALL, "");

	bindtextdomain (GETTEXT_PACKAGE, GMLIVE_LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, NULL);
	textdomain (GETTEXT_PACKAGE);

	Gtk::Main kit(argc, argv);

	MainWindow wnd; 
	kit.run();

	return 0;
}

