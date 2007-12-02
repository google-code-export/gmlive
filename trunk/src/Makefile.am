INCLUDES = 					\
						-I$(top_srcdir)				\
						-I$(srcdir)				\
						-DDATA_DIR=\""${datadir}/gmlive"\"	\
						@PACKAGE_CFLAGS@

bin_PROGRAMS = gmlive
gmlive_SOURCES = \
		 ec_throw.cpp \
		 main.cpp \
		 pst_ctrl.cpp \
		 MainWindow.cpp \
		 channel.cpp \
		 nslivechannel.cpp \
		 mmschannel.cpp \
		 gmplayer.cpp

noinst_HEADERS = \
		 ec_throw.h \
		 pst_ctrl.h \
		 MainWindow.h \
		 channel.h \
		 nslivechannel.h \
		 mmschannel.h \
		 gmplayer.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
