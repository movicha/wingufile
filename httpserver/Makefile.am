
AM_CFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DWINGUFILE_SERVER \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/common \
	-I$(includedir) \
	@CCNET_CFLAGS@ \
	@WINGURPC_CFLAGS@ \
	@GLIB2_CFLAGS@ \
	@ZDB_CFLAGS@ \
	@CURL_CFLAGS@ \
	@LIBARCHIVE_CFLAGS@ \
	@MSVC_CFLAGS@ \
	-Wall

bin_PROGRAMS = httpserver

noinst_HEADERS = wingufile-session.h repo-mgr.h \
	httpserver.h access-file.h upload-file.h pack-dir.h

httpserver_SOURCES = \
	httpserver.c \
	access-file.c \
	upload-file.c \
	wingufile-session.c \
	repo-mgr.c \
	pack-dir.c \
	../common/winguf-db.c \
	../common/bitfield.c \
	../common/branch-mgr.c \
	../common/fs-mgr.c \
	../common/block-mgr.c \
	../common/block-backend.c \
	../common/block-backend-fs.c \
	../common/block-backend-ceph.c \
	../common/commit-mgr.c \
	../common/log.c \
	../common/avl/avl.c \
	../common/object-list.c \
	../common/winguf-utils.c \
	../common/obj-store.c \
	../common/obj-backend-fs.c \
	../common/obj-backend-riak.c \
	../common/riak-http-client.c \
	../common/wingufile-crypt.c

# XXX: -levent_openssl must be behind in -levhtp
httpserver_LDADD = -levent -levhtp -lssl -levent_openssl \
	@GLIB2_LIBS@ @GOBJECT_LIBS@ @LIB_RT@ \
	@CCNET_LIBS@ \
	$(top_builddir)/lib/libwingufile.la \
	$(top_builddir)/common/cdc/libcdc.la \
	@WINGURPC_LIBS@ @ZDB_LIBS@ @RADOS_LIBS@ @CURL_LIBS@ \
	@LIBARCHIVE_LIBS@ ${LIB_WS32}

httpserver_LDFLAGS = @STATIC_COMPILE@
