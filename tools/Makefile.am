
#AM_CPPFLAGS = @GLIB2_CFLAGS@

bin_PROGRAMS = winguf-server-init

winguf_server_init_SOURCES = winguf-server-init.c ../common/winguf-db.c

winguf_server_init_LDADD = @GLIB2_LIBS@ @ZDB_LIBS@ -lpthread
winguf_server_init_LDFLAGS = @STATIC_COMPILE@ @SERVER_PKG_RPATH@

winguf_server_init_CPPFLAGS = @GLIB2_CFLAGS@ @ZDB_CFLAGS@ @MSVC_CFLAGS@

EXTRA_DIST = wingufile-admin

if COMPILE_SERVER
bin_SCRIPTS = wingufile-admin
endif
