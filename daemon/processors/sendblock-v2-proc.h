/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SENDBLOCK_V2_PROC_H
#define WINGUFILE_SENDBLOCK_V2_PROC_H

#include <glib-object.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_SENDBLOCK_V2_PROC                  (wingufile_sendblock_v2_proc_get_type ())
#define WINGUFILE_SENDBLOCK_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SENDBLOCK_V2_PROC, WingufileSendblockV2Proc))
#define WINGUFILE_IS_SENDBLOCK_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SENDBLOCK_V2_PROC))
#define WINGUFILE_SENDBLOCK_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SENDBLOCK_V2_PROC, WingufileSendblockV2ProcClass))
#define IS_WINGUFILE_SENDBLOCK_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SENDBLOCK_V2_PROC))
#define WINGUFILE_SENDBLOCK_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SENDBLOCK_V2_PROC, WingufileSendblockV2ProcClass))

typedef struct _WingufileSendblockV2Proc WingufileSendblockV2Proc;
typedef struct _WingufileSendblockV2ProcClass WingufileSendblockV2ProcClass;

struct _WingufileSendblockV2Proc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
    Bitfield       active;
    Bitfield       block_bitmap;

    int            tx_bytes;
    int            tx_time;
    double         avg_tx_rate;
    int            pending_blocks;
};

struct _WingufileSendblockV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sendblock_v2_proc_get_type ();

int wingufile_sendblock_v2_proc_send_block (WingufileSendblockV2Proc *proc,
                                          int block_idx);

gboolean wingufile_sendblock_v2_proc_is_ready (WingufileSendblockV2Proc *proc);

#endif
