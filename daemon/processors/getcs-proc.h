/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETCS_PROC_H
#define WINGUFILE_GETCS_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_GETCS_PROC                  (wingufile_getcs_proc_get_type ())
#define WINGUFILE_GETCS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETCS_PROC, WingufileGetcsProc))
#define WINGUFILE_IS_GETCS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETCS_PROC))
#define WINGUFILE_GETCS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETCS_PROC, WingufileGetcsProcClass))
#define IS_WINGUFILE_GETCS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETCS_PROC))
#define WINGUFILE_GETCS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETCS_PROC, WingufileGetcsProcClass))

typedef struct _WingufileGetcsProc WingufileGetcsProc;
typedef struct _WingufileGetcsProcClass WingufileGetcsProcClass;

struct _WingufileGetcsProc {
    CcnetProcessor parent_instance;

    TransferTask *task;
};

struct _WingufileGetcsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getcs_proc_get_type ();

#endif

