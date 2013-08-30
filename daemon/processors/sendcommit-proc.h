/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SENDCOMMIT_PROC_H
#define WINGUFILE_SENDCOMMIT_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_SENDCOMMIT_PROC                  (wingufile_sendcommit_proc_get_type ())
#define WINGUFILE_SENDCOMMIT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SENDCOMMIT_PROC, SeafileSendcommitProc))
#define WINGUFILE_IS_SENDCOMMIT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SENDCOMMIT_PROC))
#define WINGUFILE_SENDCOMMIT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SENDCOMMIT_PROC, SeafileSendcommitProcClass))
#define IS_WINGUFILE_SENDCOMMIT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SENDCOMMIT_PROC))
#define WINGUFILE_SENDCOMMIT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SENDCOMMIT_PROC, SeafileSendcommitProcClass))

typedef struct _SeafileSendcommitProc SeafileSendcommitProc;
typedef struct _SeafileSendcommitProcClass SeafileSendcommitProcClass;

struct _SeafileSendcommitProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _SeafileSendcommitProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sendcommit_proc_get_type ();

#endif
