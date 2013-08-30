bin_PROGRAMS = wingufile-controller

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/common \
	@CCNET_CFLAGS@ \
	@SEARPC_CFLAGS@ \
	@GLIB2_CFLAGS@ \
	@ZDB_CFLAGS@ \
	-Wall

noinst_HEADERS = wingufile-controller.h ../common/log.h

wingufile_controller_SOURCES = wingufile-controller.c ../common/log.c

wingufile_controller_LDADD = @CCNET_LIBS@ \
	$(top_builddir)/lib/libwingufile_common.la \
	@GLIB2_LIBS@  @GOBJECT_LIBS@ -lssl @LIB_RT@ @LIB_UUID@ -levent \
	@SEARPC_LIBS@

wingufile_controller_LDFLAGS = @STATIC_COMPILE@ @SERVER_PKG_RPATH@
