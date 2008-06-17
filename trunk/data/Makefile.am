gmlive_ui = \
	    gmlive.glade \
	    record.glade \
	    mms.lst 	\
	    gmlive.png  \
	    gmlive_play.png \
	    gmlive_big.png \
	    OpenURL.png \
	    volume.png  \
	    fullscreen.png \
	    show_channels.png

desktopdir       = $(datadir)/applications
desktop_in_files = gmlive.desktop.in
desktop_DATA     = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@


pixmapsdir = $(datadir)/pixmaps
pixmaps_DATA = gmlive.png

gmliveuidir = $(pkgdatadir)
gmliveui_DATA = $(gmlive_ui)

EXTRA_DIST = \
	     $(gmlive_ui)	\
	     $(desktop_in_files)
