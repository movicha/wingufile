/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECK_TX_PROC_H
#define WINGUFILE_CHECK_TX_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#include "transfer-mgr.h"

enum {
    CHECK_TX_TYPE_UPLOAD,
    CHECK_TX_TYPE_DOWNLOAD,
};

#define WINGUFILE_TYPE_CHECK_TX_PROC                  (wingufile_check_tx_proc_get_type ())
#define WINGUFILE_CHECK_TX_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECK_TX_PROC, WingufileCheckTxProc))
#define WINGUFILE_IS_CHECK_TX_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECK_TX_PROC))
#define WINGUFILE_CHECK_TX_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECK_TX_PROC, WingufileCheckTxProcClass))
#define IS_WINGUFILE_CHECK_TX_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECK_TX_PROC))
#define WINGUFILE_CHECK_TX_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECK_TX_PROC, WingufileCheckTxProcClass))

typedef struct _WingufileCheckTxProc WingufileCheckTxProc;
typedef struct _WingufileCheckTxProcClass WingufileCheckTxProcClass;

struct _WingufileCheckTxProc {
    CcnetProcessor parent_instance;

    int           type;
    TransferTask *task;
};

struct _WingufileCheckTxProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_check_tx_proc_get_type ();

#endif

