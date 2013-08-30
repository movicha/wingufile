/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTFS_PROC_H
#define WINGUFILE_PUTFS_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTFS_PROC                  (wingufile_putfs_proc_get_type ())
#define WINGUFILE_PUTFS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTFS_PROC, SeafilePutfsProc))
#define WINGUFILE_IS_PUTFS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTFS_PROC))
#define WINGUFILE_PUTFS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTFS_PROC, SeafilePutfsProcClass))
#define IS_WINGUFILE_PUTFS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTFS_PROC))
#define WINGUFILE_PUTFS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTFS_PROC, SeafilePutfsProcClass))

typedef struct _SeafilePutfsProc SeafilePutfsProc;
typedef struct _SeafilePutfsProcClass SeafilePutfsProcClass;

struct _SeafilePutfsProc {
    CcnetProcessor parent_instance;
};

struct _SeafilePutfsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putfs_proc_get_type ();

#endif

