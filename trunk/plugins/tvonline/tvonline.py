#! /usr/bin/env python
# -*- encoding:utf-8 -*-
# FileName: tvonline.py

"This file is part of ____"
 
__author__   = "yetist"
__copyright__= "Copyright (C) 2008 yetist <yetist@gmail.com>"
__license__  = """
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""
#

import totem
import gobject, gtk
import os

(COL_NAME,
COL_URL,
COL_CLASS
 ) = range(3)

def on_active (tv, path, view_column, totem):
    model = tv.get_model()
    iter = model.get_iter(path)
    url = model.get_value(iter, COL_URL)
    totem.action_set_mrl_and_play(url)

def create_page(totem):
    tv = gtk.TreeView()

    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_OUT)
    sw = gtk.ScrolledWindow()
    sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
    sw.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
    sw.add(tv)
    frame.add(sw)

    model = gtk.ListStore (gobject.TYPE_STRING, gobject.TYPE_STRING, gobject.TYPE_STRING)

    path = os.path.abspath(globals()['__file__'])
    path = os.path.join(os.path.dirname(path), "mms.lst")

    lines = open(path).readlines()
    for line in lines:
        if len(line.strip()) <2:
            continue
        try:
            [n,u,c] = line.split("#")
            iter = model.append (None)
            model.set(iter, 
                    COL_NAME, n,
                    COL_URL, u,
                    COL_CLASS, c)
        except:
            try:
                [n,u] = line.split("#")
                iter = model.append (None)
                model.set(iter, 
                        COL_NAME, n,
                        COL_URL, u,
                        COL_CLASS, "")
            except:
                print "has error format in file %s" % path
                continue
    tv.set_model(model)
    tv.connect("row-activated", on_active, totem)
    col = gtk.TreeViewColumn("Name", gtk.CellRendererText(), text=COL_NAME)
    tv.append_column(col)
    frame.show_all()
    return frame

class TVonline(totem.Plugin):
    def __init__(self):
        totem.Plugin.__init__(self)

    def activate(self, totem):
        totem.add_sidebar_page("tvonline", "Online TV", create_page(totem))
        #totem.action_set_mrl("http://abc")
        #totem.action_set_mrl_and_play("http://abc")
        #print dir(totem.props)
        return True

    def deactivate(self, totem):
        totem.remove_sidebar_page("tvonline")

    def create_configure_dialog(self, totem):
        conf_dlg= gtk.FileChooserDialog()
        return conf_dlg

#    def is_configurable (self, totem):
#        return True
