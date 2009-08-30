INCLUDES = \
	-I$(top_srcdir)				\
	-I$(srcdir)				\
	-DDATA_DIR=\""${datadir}/gmlive"\"	\
	-DGMLIVE_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@	

bin_PROGRAMS = gmlive
gmlive_SOURCES = \
		 ec_throw.cpp \
		 variablesmap.cpp \
		 main.cpp \
		 MainWindow.cpp \
		 ConfWindow.cpp \
		 channel.cpp \
		 ChannelsTooltips.cpp \
		 mmschannel.cpp \
		 sopcastchannel.cpp \
		 pplivechannel.cpp \
		 ppschannel.cpp \
		 recentchannel.cpp \
		 bookmarkchannel.cpp \
		 gmplayer.cpp \
		 sopcastLivePlayer.cpp \
		 pplivePlayer.cpp \
		 mmsLivePlayer.cpp \
		 ppsPlayer.cpp \
		 TrayIcon.cpp

noinst_HEADERS = \
	     scope_gruard.h \
		 ec_throw.h \
		 variablesmap.h \
		 CodeConverter.h \
		 scope_gruard.h \
		 MainWindow.h \
		 ConfWindow.h \
		 channel.h \
		 ChannelsTooltips.h \
		 sopcastchannel.h \
		 pplivechannel.h \
		 ppschannel.h \
		 mmschannel.h \
		 recentchannel.h \
		 bookmarkchannel.h \
		 gmplayer.h \
		 gmlive.h 	\
		 livePlayer.h \
		 sopcastLivePlayer.h \
		 pplivePlayer.h \
		 mmsLivePlayer.h \
		 ppsPlayer.h \
		 TrayIcon.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@ 
