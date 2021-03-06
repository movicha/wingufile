/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTREPOEMAILTOKEN_PROC_H
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC                  (wingufile_putrepoemailtoken_proc_get_type ())
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, WingufilePutrepoemailtokenProc))
#define WINGUFILE_IS_PUTREPOEMAILTOKEN_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC))
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, WingufilePutrepoemailtokenProcClass))
#define IS_WINGUFILE_PUTREPOEMAILTOKEN_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC))
#define WINGUFILE_PUTREPOEMAILTOKEN_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTREPOEMAILTOKEN_PROC, WingufilePutrepoemailtokenProcClass))

typedef struct _WingufilePutrepoemailtokenProc WingufilePutrepoemailtokenProc;
typedef struct _WingufilePutrepoemailtokenProcClass WingufilePutrepoemailtokenProcClass;

struct _WingufilePutrepoemailtokenProc {
    CcnetProcessor parent_instance;
};

struct _WingufilePutrepoemailtokenProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putrepoemailtoken_proc_get_type ();

#endif

