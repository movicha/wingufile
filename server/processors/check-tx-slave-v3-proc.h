/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECK_TX_SLAVE_V3_PROC_H
#define WINGUFILE_CHECK_TX_SLAVE_V3_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC                  (wingufile_check_tx_slave_v3_proc_get_type ())
#define WINGUFILE_CHECK_TX_SLAVE_V3_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC, WingufileCheckTxSlaveV3Proc))
#define WINGUFILE_IS_CHECK_TX_SLAVE_V3_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_V3_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC, WingufileCheckTxSlaveV3ProcClass))
#define IS_WINGUFILE_CHECK_TX_SLAVE_V3_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_V3_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC, WingufileCheckTxSlaveV3ProcClass))

typedef struct _WingufileCheckTxSlaveV3Proc WingufileCheckTxSlaveV3Proc;
typedef struct _WingufileCheckTxSlaveV3ProcClass WingufileCheckTxSlaveV3ProcClass;

struct _WingufileCheckTxSlaveV3Proc {
    CcnetProcessor parent_instance;
};

struct _WingufileCheckTxSlaveV3ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_check_tx_slave_v3_proc_get_type ();

#endif
