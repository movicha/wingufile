
AM_CFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DWINGUFILE_SERVER \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/common \
	@CCNET_CFLAGS@ \
	@WINGURPC_CFLAGS@ \
	@GLIB2_CFLAGS@ \
	@ZDB_CFLAGS@ \
	@MSVC_CFLAGS@ \
	@CURL_CFLAGS@ \
	-Wall

bin_PROGRAMS = wingufserv-gc

noinst_HEADERS = \
	wingufile-session.h \
	repo-mgr.h \
	verify.h

wingufserv_gc_SOURCES = \
	wingufserv-gc.c \
	wingufile-session.c \
	repo-mgr.c \
	verify.c \
	../../common/gc-core.c \
	../../common/winguf-db.c \
	../../common/bitfield.c \
	../../common/branch-mgr.c \
	../../common/fs-mgr.c \
	../../common/block-mgr.c \
	../../common/block-backend.c \
	../../common/block-backend-fs.c \
	../../common/block-backend-ceph.c \
	../../common/commit-mgr.c \
	../../common/avl/avl.c \
	../../common/log.c \
	../../common/winguf-utils.c \
	../../common/obj-store.c \
	../../common/obj-backend-fs.c \
	../../common/obj-backend-riak.c \
	../../common/riak-http-client.c \
	../../common/wingufile-crypt.c

wingufserv_gc_LDADD = @CCNET_LIBS@ \
	$(top_builddir)/common/cdc/libcdc.la \
	$(top_builddir)/lib/libwingufile_common.la \
	@GLIB2_LIBS@ @GOBJECT_LIBS@ -lssl @LIB_RT@ @LIB_UUID@ -lsqlite3 -levent \
	@WINGURPC_LIBS@ @ZDB_LIBS@ @RADOS_LIBS@ @CURL_LIBS@ ${LIB_WS32}

wingufserv_gc_LDFLAGS = @STATIC_COMPILE@ @SERVER_PKG_RPATH@
