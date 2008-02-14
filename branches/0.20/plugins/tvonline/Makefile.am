plugindir = $(PLUGINDIR)/tvonline
plugin_PYTHON = tvonline.py

plugin_in_files = tvonline.totem-plugin.in

%.totem-plugin: %.totem-plugin.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.totem-plugin.in=.totem-plugin) mms.lst

EXTRA_DIST = $(plugin_in_files) $(plugin_PYTHON) mms.lst

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

