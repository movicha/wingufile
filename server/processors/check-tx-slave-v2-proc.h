/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECK_TX_SLAVE_V2_PROC_H
#define WINGUFILE_CHECK_TX_SLAVE_V2_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC                  (wingufile_check_tx_slave_v2_proc_get_type ())
#define WINGUFILE_CHECK_TX_SLAVE_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC, SeafileCheckTxSlaveV2Proc))
#define WINGUFILE_IS_CHECK_TX_SLAVE_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC, SeafileCheckTxSlaveV2ProcClass))
#define IS_WINGUFILE_CHECK_TX_SLAVE_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC, SeafileCheckTxSlaveV2ProcClass))

typedef struct _SeafileCheckTxSlaveV2Proc SeafileCheckTxSlaveV2Proc;
typedef struct _SeafileCheckTxSlaveV2ProcClass SeafileCheckTxSlaveV2ProcClass;

struct _SeafileCheckTxSlaveV2Proc {
    CcnetProcessor parent_instance;
};

struct _SeafileCheckTxSlaveV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_check_tx_slave_v2_proc_get_type ();

#endif
