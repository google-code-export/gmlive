/*
 * =====================================================================================
 * 
 *       Filename:  confwindow.h
 * 
 *    Description:  config class for gmlive
 * 
 *        Version:  1.0
 *        Created:  2007年12月17日 21时25分41秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 * 
 * =====================================================================================
 */

#ifndef  CONFWINDOW_FILE_HEADER_INC
#define  CONFWINDOW_FILE_HEADER_INC

#include "gmlive.h"
class MainWindow;
class ConfWindow: public Gtk::Window
{

	public:
		ConfWindow(MainWindow* parent_);
		~ConfWindow(){}
		

		void on_button_save();
		void on_button_cancel();
		bool on_key_press_event(GdkEventKey* ev);
	private:
		MainWindow* parent;






};
#endif   /* ----- #ifndef CONFWINDOW_FILE_HEADER_INC  ----- */

