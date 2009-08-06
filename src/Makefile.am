INCLUDES = \
	-I$(top_srcdir)				\
	-I$(srcdir)				\
	-DDATA_DIR=\""${datadir}/gmlive"\"	\
	-DGMLIVE_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@	

bin_PROGRAMS = gmlive
gmlive_SOURCES = \
		 ec_throw.cpp \
		 main.cpp \
		 MainWindow.cpp \
		 ConfWindow.cpp \
		 channel.cpp \
		 ChannelsTooltips.cpp \
		 mmschannel.cpp \
		 sopcastchannel.cpp \
		 pplivechannel.cpp \
		 recentchannel.cpp \
		 bookmarkchannel.cpp \
		 gmplayer.cpp \
		 sopcastLivePlayer.cpp \
		 mmsLivePlayer.cpp \
		 playStream.cpp \
		 recordStream.cpp \
		 TrayIcon.cpp

noinst_HEADERS = \
	     scope_gruard.h \
		 ec_throw.h \
		 CodeConverter.h \
		 scope_gruard.h \
		 MainWindow.h \
		 ConfWindow.h \
		 channel.h \
		 ChannelsTooltips.h \
		 sopcastchannel.h \
		 pplivechannel.h \
		 mmschannel.h \
		 recentchannel.h \
		 bookmarkchannel.h \
		 gmplayer.h \
		 gmlive.h 	\
		 livePlayer.h \
		 sopcastLivePlayer.h \
		 mmsLivePlayer.h \
		 playStream.h \
		 recordStream.h \
		 TrayIcon.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
