/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETCOMMIT_PROC_H
#define WINGUFILE_GETCOMMIT_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_GETCOMMIT_PROC                  (wingufile_getcommit_proc_get_type ())
#define WINGUFILE_GETCOMMIT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC, WingufileGetcommitProc))
#define WINGUFILE_IS_GETCOMMIT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC))
#define WINGUFILE_GETCOMMIT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETCOMMIT_PROC, WingufileGetcommitProcClass))
#define IS_WINGUFILE_GETCOMMIT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETCOMMIT_PROC))
#define WINGUFILE_GETCOMMIT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC, WingufileGetcommitProcClass))

typedef struct _WingufileGetcommitProc WingufileGetcommitProc;
typedef struct _WingufileGetcommitProcClass WingufileGetcommitProcClass;

struct _WingufileGetcommitProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _WingufileGetcommitProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getcommit_proc_get_type ();

#endif
