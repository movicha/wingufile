/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETCOMMIT_V2_PROC_H
#define WINGUFILE_GETCOMMIT_V2_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_GETCOMMIT_V2_PROC                  (wingufile_getcommit_v2_proc_get_type ())
#define WINGUFILE_GETCOMMIT_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETCOMMIT_V2_PROC, WingufileGetcommitV2Proc))
#define WINGUFILE_IS_GETCOMMIT_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETCOMMIT_V2_PROC))
#define WINGUFILE_GETCOMMIT_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETCOMMIT_V2_PROC, WingufileGetcommitV2ProcClass))
#define IS_WINGUFILE_GETCOMMIT_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETCOMMIT_V2_PROC))
#define WINGUFILE_GETCOMMIT_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETCOMMIT_V2_PROC, WingufileGetcommitV2ProcClass))

typedef struct _WingufileGetcommitV2Proc WingufileGetcommitV2Proc;
typedef struct _WingufileGetcommitV2ProcClass WingufileGetcommitV2ProcClass;

struct _WingufileGetcommitV2Proc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _WingufileGetcommitV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getcommit_v2_proc_get_type ();

#endif
