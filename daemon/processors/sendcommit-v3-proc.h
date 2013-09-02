/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SENDCOMMIT_V3_PROC_H
#define WINGUFILE_SENDCOMMIT_V3_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_SENDCOMMIT_V3_PROC                  (wingufile_sendcommit_v3_proc_get_type ())
#define WINGUFILE_SENDCOMMIT_V3_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SENDCOMMIT_V3_PROC, WingufileSendcommitV3Proc))
#define WINGUFILE_IS_SENDCOMMIT_V3_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SENDCOMMIT_V3_PROC))
#define WINGUFILE_SENDCOMMIT_V3_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SENDCOMMIT_V3_PROC, WingufileSendcommitV3ProcClass))
#define IS_WINGUFILE_SENDCOMMIT_V3_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SENDCOMMIT_V3_PROC))
#define WINGUFILE_SENDCOMMIT_V3_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SENDCOMMIT_V3_PROC, WingufileSendcommitV3ProcClass))

typedef struct _WingufileSendcommitV3Proc WingufileSendcommitV3Proc;
typedef struct _WingufileSendcommitV3ProcClass WingufileSendcommitV3ProcClass;

struct _WingufileSendcommitV3Proc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
};

struct _WingufileSendcommitV3ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sendcommit_v3_proc_get_type ();

#endif
