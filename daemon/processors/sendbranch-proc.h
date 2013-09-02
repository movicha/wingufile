/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SENDBRANCH_PROC_H
#define WINGUFILE_SENDBRANCH_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#include "transfer-mgr.h"

#define WINGUFILE_TYPE_SENDBRANCH_PROC                  (wingufile_sendbranch_proc_get_type ())
#define WINGUFILE_SENDBRANCH_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SENDBRANCH_PROC, WingufileSendbranchProc))
#define WINGUFILE_IS_SENDBRANCH_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SENDBRANCH_PROC))
#define WINGUFILE_SENDBRANCH_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SENDBRANCH_PROC, WingufileSendbranchProcClass))
#define IS_WINGUFILE_SENDBRANCH_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SENDBRANCH_PROC))
#define WINGUFILE_SENDBRANCH_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SENDBRANCH_PROC, WingufileSendbranchProcClass))

typedef struct _WingufileSendbranchProc WingufileSendbranchProc;
typedef struct _WingufileSendbranchProcClass WingufileSendbranchProcClass;

struct _WingufileSendbranchProc {
    CcnetProcessor parent_instance;

    TransferTask *task;
};

struct _WingufileSendbranchProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sendbranch_proc_get_type ();

#endif

