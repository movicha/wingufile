/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTREPOEMAILTOKEN_PROC_H
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC                  (wingufile_putrepoemailtoken_proc_get_type ())
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, SeafilePutrepoemailtokenProc))
#define WINGUFILE_IS_PUTREPOEMAILTOKEN_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC))
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, SeafilePutrepoemailtokenProcClass))
#define IS_WINGUFILE_PUTREPOEMAILTOKEN_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC))
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, SeafilePutrepoemailtokenProcClass))

typedef struct _SeafilePutrepoemailtokenProc SeafilePutrepoemailtokenProc;
typedef struct _SeafilePutrepoemailtokenProcClass SeafilePutrepoemailtokenProcClass;

struct _SeafilePutrepoemailtokenProc {
    CcnetProcessor parent_instance;
};

struct _SeafilePutrepoemailtokenProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putrepoemailtoken_proc_get_type ();

#endif

