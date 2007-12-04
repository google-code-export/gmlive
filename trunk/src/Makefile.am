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
		 recentchannel.cpp \
		 gmplayer.cpp \
		 mms_live_player.cpp \
		 ns_live_player.cpp

noinst_HEADERS = \
		 ec_throw.h \
		 pst_ctrl.h \
		 MainWindow.h \
		 channel.h \
		 nslivechannel.h \
		 mmschannel.h \
		 recentchannel.h \
		 gmplayer.h \
		 gmlive.h 	\
		 live_player.h \
		 mms_live_player.h \
		 ns_live_player.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
