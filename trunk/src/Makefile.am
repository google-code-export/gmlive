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
		 ConfWindow.cpp \
		 StreamMenu.cpp \
		 channel.cpp \
		 nslivechannel.cpp \
		 ChannelsTooltips.cpp \
		 mmschannel.cpp \
		 sopcastchannel.cpp \
		 recentchannel.cpp \
		 bookmarkchannel.cpp \
		 gmplayer.cpp \
		 sopcastLivePlayer.cpp \
		 mmsLivePlayer.cpp \
		 nsLivePlayer.cpp \
		 playStream.cpp \
		 recordStream.cpp

noinst_HEADERS = \
		 ec_throw.h \
		 CodeConverter.h \
		 scope_gruard.h \
		 MainWindow.h \
		 ConfWindow.h \
		 StreamMenu.h \
		 channel.h \
		 ChannelsTooltips.h \
		 sopcastchannel.h \
		 nslivechannel.h \
		 mmschannel.h \
		 recentchannel.h \
		 bookmarkchannel.h \
		 gmplayer.h \
		 gmlive.h 	\
		 livePlayer.h \
		 sopcastLivePlayer.h \
		 mmsLivePlayer.h \
		 nsLivePlayer.h \
		 playStream.h \
		 recordStream.h

gmlive_LDADD = @LIBS@ @PACKAGE_LIBS@
