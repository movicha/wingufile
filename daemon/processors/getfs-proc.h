/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETFS_PROC_H
#define WINGUFILE_GETFS_PROC_H

#include <glib-object.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_GETFS_PROC                  (wingufile_getfs_proc_get_type ())
#define WINGUFILE_GETFS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETFS_PROC, WingufileGetfsProc))
#define WINGUFILE_IS_GETFS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETFS_PROC))
#define WINGUFILE_GETFS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETFS_PROC, WingufileGetfsProcClass))
#define IS_WINGUFILE_GETFS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETFS_PROC))
#define WINGUFILE_GETFS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETFS_PROC, WingufileGetfsProcClass))

typedef struct _WingufileGetfsProc WingufileGetfsProc;
typedef struct _WingufileGetfsProcClass WingufileGetfsProcClass;

struct _WingufileGetfsProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _WingufileGetfsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getfs_proc_get_type ();

#endif

