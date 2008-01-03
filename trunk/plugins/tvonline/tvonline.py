#! /usr/bin/env python
# -*- encoding:utf-8 -*-
# FileName: tvonline.py

"This file is plugin for totem."
 
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
url="mms://live.njbg.com.cn:88/NJTV01"

import totem
import gobject, gtk

class tvonline(totem.Plugin):

    def __init__(self):
        totem.Plugin.__init__(self)

    def activate(self, totem):
        page = gtk.Label("hi all! It's just a test")
        totem.add_sidebar_page("tv", "在线电视", page)
        return True

    def deactivate(self, totem):
        pass
