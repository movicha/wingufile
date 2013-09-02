/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECKBL_PROC_H
#define WINGUFILE_CHECKBL_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#include "transfer-mgr.h"

#define WINGUFILE_TYPE_CHECKBL_PROC                  (wingufile_checkbl_proc_get_type ())
#define WINGUFILE_CHECKBL_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECKBL_PROC, WingufileCheckblProc))
#define WINGUFILE_IS_CHECKBL_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECKBL_PROC))
#define WINGUFILE_CHECKBL_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECKBL_PROC, WingufileCheckblProcClass))
#define IS_WINGUFILE_CHECKBL_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECKBL_PROC))
#define WINGUFILE_CHECKBL_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECKBL_PROC, WingufileCheckblProcClass))

typedef struct _WingufileCheckblProc WingufileCheckblProc;
typedef struct _WingufileCheckblProcClass WingufileCheckblProcClass;

struct _WingufileCheckblProc {
    CcnetProcessor parent_instance;

    TransferTask *task;
};

struct _WingufileCheckblProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_checkbl_proc_get_type ();

#endif

