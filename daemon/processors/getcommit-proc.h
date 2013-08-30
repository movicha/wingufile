/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETCOMMIT_PROC_H
#define WINGUFILE_GETCOMMIT_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_GETCOMMIT_PROC                  (wingufile_getcommit_proc_get_type ())
#define WINGUFILE_GETCOMMIT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC, SeafileGetcommitProc))
#define WINGUFILE_IS_GETCOMMIT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC))
#define WINGUFILE_GETCOMMIT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETCOMMIT_PROC, SeafileGetcommitProcClass))
#define IS_WINGUFILE_GETCOMMIT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETCOMMIT_PROC))
#define WINGUFILE_GETCOMMIT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETCOMMIT_PROC, SeafileGetcommitProcClass))

typedef struct _SeafileGetcommitProc SeafileGetcommitProc;
typedef struct _SeafileGetcommitProcClass SeafileGetcommitProcClass;

struct _SeafileGetcommitProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _SeafileGetcommitProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getcommit_proc_get_type ();

#endif
