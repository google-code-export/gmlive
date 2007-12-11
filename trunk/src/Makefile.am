INCLUDES = 					\
						-I$(top_srcdir)				\
						-I$(srcdir)				\
						-DDATA_DIR=\""${datadir}/gmlive"\"	\
						@PACKAGE_CFLAGS@

bin_PROGRAMS = gmlive
gmlive_SOURCES = \
		 ec_throw.cpp \
		 main.cpp \
		 MainWindow.cpp \
		 StreamMenu.cpp \
		 channel.cpp \
		 nslivechannel.cpp \
		 mmschannel.cpp \
		 sopcastchannel.cpp \
		 recentchannel.cpp \
		 bookmarkchannel.cpp \
		 gmplayer.cpp \
		 ConfFile.cpp \
		 sopcast_live_player.cpp \
		 mms_live_player.cpp \
		 ns_live_player.cpp

noinst_HEADERS = \
		 ec_throw.h \
		 CodeConverter.h \
		 scope_gruard.h \
		 MainWindow.h \
		 StreamMenu.h \
		 channel.h \
		 sopcastchannel.h \
		 nslivechannel.h \
		 mmschannel.h \
		 recentchannel.h \
		 bookmarkchannel.h \
		 gmplayer.h \
		 gmlive.h 	\
		 ConfFile.h 	\
		 live_player.h \
		 sopcast_live_player.h \
		 mms_live_player.h \
		 ns_live_player.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
