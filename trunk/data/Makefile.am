desktopdir       = $(datadir)/applications
desktop_in_files = gmlive.desktop.in
desktop_DATA     = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

Iconsdir = $(datadir)/pixmaps
Icons_DATA = gmlive.png

CLEANFILES =	\
		$(desktop_DATA)

gmlive_ui = \
	    gmlive.xml \
	    mms.lst 	\
	    gmlive_big.png

EXTRA_DIST = \
	     $(gmlive_ui)
gmliveuidir = $(pkgdatadir)
gmliveui_DATA = $(EXTRA_DIST) \
		$(Icons_DATA)
