/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECK_TX_V3_PROC_H
#define WINGUFILE_CHECK_TX_V3_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#include "transfer-mgr.h"

#define WINGUFILE_TYPE_CHECK_TX_V3_PROC               (wingufile_check_tx_v3_proc_get_type ())
#define WINGUFILE_CHECK_TX_V3_PROC(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECK_TX_V3_PROC, WingufileCheckTxV3Proc))
#define WINGUFILE_IS_CHECK_TX_V3_PROC(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECK_TX_PROC))
#define WINGUFILE_CHECK_TX_V3_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECK_TX_V3_PROC, WingufileCheckTxV3ProcClass))
#define IS_WINGUFILE_CHECK_TX_V3_PROC_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECK_TX_V3_PROC))
#define WINGUFILE_CHECK_TX_V3_PROC_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECK_TX_V3_PROC, WingufileCheckTxV3ProcClass))

typedef struct _WingufileCheckTxV3Proc WingufileCheckTxV3Proc;
typedef struct _WingufileCheckTxV3ProcClass WingufileCheckTxV3ProcClass;

struct _WingufileCheckTxV3Proc {
    CcnetProcessor parent_instance;

    int           type;
    TransferTask *task;
};

struct _WingufileCheckTxV3ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_check_tx_v3_proc_get_type ();

#endif
