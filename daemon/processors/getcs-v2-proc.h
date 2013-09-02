/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETCS_V2_PROC_H
#define WINGUFILE_GETCS_V2_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_GETCS_V2_PROC                  (wingufile_getcs_v2_proc_get_type ())
#define WINGUFILE_GETCS_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETCS_V2_PROC, WingufileGetcsV2Proc))
#define WINGUFILE_IS_GETCS_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETCS_V2_PROC))
#define WINGUFILE_GETCS_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETCS_V2_PROC, WingufileGetcsV2ProcClass))
#define IS_WINGUFILE_GETCS_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETCS_V2_PROC))
#define WINGUFILE_GETCS_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETCS_V2_PROC, WingufileGetcsV2ProcClass))

typedef struct _WingufileGetcsV2Proc WingufileGetcsV2Proc;
typedef struct _WingufileGetcsV2ProcClass WingufileGetcsV2ProcClass;

struct _WingufileGetcsV2Proc {
    CcnetProcessor parent_instance;

    TransferTask *task;
};

struct _WingufileGetcsV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getcs_v2_proc_get_type ();

#endif

