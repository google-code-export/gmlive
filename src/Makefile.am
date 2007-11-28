INCLUDES = 					\
	-I$(top_srcdir)				\
	-I$(srcdir)				\
	-DDATA_DIR=\""${datadir}/gmmlive"\"	\
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gmmlive
gmmlive_SOURCES = \
		  main.cpp \
		  MainWindow.cpp \
		  gmmplayer.cpp

noinst_HEADERS = \
		MainWindow.h \
		gmmplayer.h

gmmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
