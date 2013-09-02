/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CHECK_TX_SLAVE_PROC_H
#define WINGUFILE_CHECK_TX_SLAVE_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC                  (wingufile_check_tx_slave_proc_get_type ())
#define WINGUFILE_CHECK_TX_SLAVE_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC, WingufileCheckTxSlaveProc))
#define WINGUFILE_IS_CHECK_TX_SLAVE_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC, WingufileCheckTxSlaveProcClass))
#define IS_WINGUFILE_CHECK_TX_SLAVE_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC))
#define WINGUFILE_CHECK_TX_SLAVE_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_CHECK_TX_SLAVE_PROC, WingufileCheckTxSlaveProcClass))

typedef struct _WingufileCheckTxSlaveProc WingufileCheckTxSlaveProc;
typedef struct _WingufileCheckTxSlaveProcClass WingufileCheckTxSlaveProcClass;

struct _WingufileCheckTxSlaveProc {
    CcnetProcessor parent_instance;
};

struct _WingufileCheckTxSlaveProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_check_tx_slave_proc_get_type ();

#endif

