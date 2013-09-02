AM_CFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/lib  \
	@CCNET_CFLAGS@ \
	@GLIB2_CFLAGS@ \
	@MSVC_CFLAGS@ \
	-Wall

bin_PROGRAMS = wingufile
if COMPILE_SERVER
bin_PROGRAMS += wingufserv-tool
endif

if COMPILE_CLIENT
bin_SCRIPTS = winguf-cli
endif

wingufile_SOURCES = wingufile.c
wingufile_LDADD =  @CCNET_LIBS@ \
	$(top_builddir)/lib/libwingufile.la \
	$(top_builddir)/lib/libwingufile_common.la \
	@WINGURPC_LIBS@ \
	@GLIB2_LIBS@  @GOBJECT_LIBS@ -lssl @LIB_RT@ @LIB_UUID@ -lsqlite3

wingufile_LDFALGS = @STATIC_COMPILE@ @CONSOLE@

if COMPILE_SERVER
wingufserv_tool_SOURCES = wingufserv-tool.c
wingufserv_tool_LDADD =  @CCNET_LIBS@ \
	$(top_builddir)/lib/libwingufile.la @WINGURPC_LIBS@ \
	@GLIB2_LIBS@  @GOBJECT_LIBS@ -lssl @LIB_RT@ @LIB_UUID@ -lsqlite3

wingufserv_tool_LDFLAGS = @STATIC_COMPILE@ @CONSOLE@ @SERVER_PKG_RPATH@
endif

# monitor_tool_SOURCES = monitor-tool.c
# monitor_tool_LDADD = @CCNET_CFLAGS@ \
# 	-lwingurpc \
# 	@GLIB2_LIBS@  @GOBJECT_LIBS@ -lssl -lrt -luuid -lsqlite3

EXTRA_DIST = winguf-cli