/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SENDFS_PROC_H
#define WINGUFILE_SENDFS_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_SENDFS_PROC                  (wingufile_sendfs_proc_get_type ())
#define WINGUFILE_SENDFS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SENDFS_PROC, WingufileSendfsProc))
#define WINGUFILE_IS_SENDFS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SENDFS_PROC))
#define WINGUFILE_SENDFS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SENDFS_PROC, WingufileSendfsProcClass))
#define IS_WINGUFILE_SENDFS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SENDFS_PROC))
#define WINGUFILE_SENDFS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SENDFS_PROC, WingufileSendfsProcClass))

typedef struct _WingufileSendfsProc WingufileSendfsProc;
typedef struct _WingufileSendfsProcClass WingufileSendfsProcClass;

struct _WingufileSendfsProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
    int last_idx;               /* used in send root fs to peer */
};

struct _WingufileSendfsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sendfs_proc_get_type ();

#endif

