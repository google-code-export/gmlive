/*
 * =====================================================================================
 * 
 *       Filename:  StreamMenu.h
 * 
 *    Description:  列表里的菜单
 * 
 *        Version:  1.0
 *        Created:  2007年12月04日 21时13分48秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  STREAMMENU_FILE_HEADER_INC
#define  STREAMMENU_FILE_HEADER_INC
#include <gtkmm.h>
#include <gtkmm/menu.h>

class MainWindow;

class StreamMenu: public Gtk::Menu
{
	public:
		StreamMenu(MainWindow& wnd);
};


#endif   /* ----- #ifndef STREAMMENU_FILE_HEADER_INC  ----- */

