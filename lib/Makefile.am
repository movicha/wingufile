pcfiles = libwingufile.pc
pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = @GLIB2_CFLAGS@ -I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/common \
	-DG_LOG_DOMAIN=\"Seafile\" \
	@CCNET_CFLAGS@ \
	@SEARPC_CFLAGS@ \
	@MSVC_CFLAGS@ \
	-Wall

BUILT_SOURCES = gensource

## source file rules
wingufile_object_define = repo.vala commit.vala dirent.vala dir.vala \
	task.vala branch.vala crypt.vala webaccess.vala

wingufile_object_gen = $(wingufile_object_define:.vala=.c)

EXTRA_DIST = ${wingufile_object_define} rpc_table.py $(pcfiles)

utils_headers = net.h rsa.h bloom-filter.h utils.h db.h

utils_srcs = $(utils_headers:.h=.c)

noinst_HEADERS = ${utils_headers} include.h

wingufiledir = $(includedir)/wingufile
wingufile_HEADERS = wingufile-object.h

wingufile-rpc-wrapper.c: wingufile-object.h

.vala.c:
	rm -f $@
	valac -C --pkg posix ${wingufile_object_define}

wingufile-object.h: ${wingufile_object_define}
	rm -f $@
	valac --pkg posix ${wingufile_object_define} -C -H wingufile-object.h

DISTCLEANFILES = ${searpc_gen}

## library rules
lib_LTLIBRARIES = libwingufile.la

libwingufile_la_SOURCES = ${wingufile_object_gen} wingufile-rpc-wrapper.c

libwingufile_la_LDFLAGS = -no-undefined
libwingufile_la_LIBADD = @GLIB2_LIBS@  @GOBJECT_LIBS@ @SEARPC_LIBS@

noinst_LTLIBRARIES = libwingufile_common.la

libwingufile_common_la_SOURCES = ${wingufile_object_gen} ${utils_srcs}
libwingufile_common_la_LDFLAGS = -no-undefined
libwingufile_common_la_LIBADD = @GLIB2_LIBS@  @GOBJECT_LIBS@ -lssl -lcrypto @LIB_GDI32@ \
				     @LIB_UUID@ @LIB_WS32@ @LIB_PSAPI@ -lsqlite3 \
					 -levent @SEARPC_LIBS@ @LIB_SHELL32@

searpc_gen = searpc-signature.h searpc-marshal.h

gensource: ${searpc_gen}

${searpc_gen}: $(top_srcdir)/lib/rpc_table.py
	@echo "[libsearpc]: generating rpc header files"
	@PYTHON@ `which searpc-codegen.py` $(top_srcdir)/lib/rpc_table.py
	@echo "[libsearpc]: done"

clean-local:
	rm -f ${searpc_gen}
	rm -f $(top_srcdir)/lib/rpc_table.pyc

install-data-local:
if MACOS
	${SED} -i '' -e "s|(DESTDIR)|${DESTDIR}|g" $(pcfiles)
else
	${SED} -i "s|(DESTDIR)|${DESTDIR}|g" $(pcfiles)
endif
