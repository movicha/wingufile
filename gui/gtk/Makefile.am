
AM_CFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DWINGUFILE_LOCALE_DIR=\""$(wingufilelocaledir)"\" \
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/gui/common \
	@CCNET_CFLAGS@ \
	@LIBNOTIFY_CFLAGS@ @GTK_CFLAGS@ @APP_INDICATOR_CFLAGS@ \
	@WINGURPC_CFLAGS@

noinst_HEADERS = trayicon.h misc.h \
	../common/ccnet-init.h \
	../common/opendir-proc.h ../common/applet-rpc-service.h \
	../common/applet-log.h ../common/rpc-wrapper.h \
	../common/applet-common.h ../common/wingufile-applet.h \
	../common/translate-commit-desc.h

bin_PROGRAMS = wingufile-applet

wingufile_applet_SOURCES = wingufile-applet.c trayicon.c \
    init-ccnet.c misc.c \
	../common/opendir-proc.c ../common/applet-log.c \
	../common/ccnet-init.c ../common/rpc-wrapper.c \
	../common/applet-common.c ../common/applet-rpc-service.c \
	../common/translate-commit-desc.c

wingufile_applet_LDADD  = @CCNET_LIBS@  \
	$(top_builddir)/lib/libwingufile.la \
	$(top_builddir)/lib/libwingufile_common.la \
	@GTK_LIBS@ -lssl -lcrypto @LIB_RT@ @LIB_UUID@ \
	-lsqlite3 -levent \
	$(LIBNOTIFY_LIBS) $(APP_INDICATOR_LIBS) @WINGURPC_LIBS@

wingufile_applet_LDFLAGS = @STATIC_COMPILE@ @CONSOLE@

ui = wingufile-trayicon.ui init-wingufile-window.ui

uidir = $(pkgdatadir)
ui_DATA = $(ui)

EXTRA_DIST = $(ui) 
