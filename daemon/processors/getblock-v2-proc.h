/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETBLOCK_V2_PROC_H
#define WINGUFILE_GETBLOCK_V2_PROC_H

#include <glib-object.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_GETBLOCK_V2_PROC                  (wingufile_getblock_v2_proc_get_type ())
#define WINGUFILE_GETBLOCK_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETBLOCK_V2_PROC, WingufileGetblockV2Proc))
#define WINGUFILE_IS_GETBLOCK_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETBLOCK_V2_PROC))
#define WINGUFILE_GETBLOCK_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETBLOCK_V2_PROC, WingufileGetblockV2ProcClass))
#define IS_WINGUFILE_GETBLOCK_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETBLOCK_V2_PROC))
#define WINGUFILE_GETBLOCK_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETBLOCK_V2_PROC, WingufileGetblockV2ProcClass))

typedef struct _WingufileGetblockV2Proc WingufileGetblockV2Proc;
typedef struct _WingufileGetblockV2ProcClass WingufileGetblockV2ProcClass;

struct _WingufileGetblockV2Proc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
    Bitfield       active;       /* what blocks to download from the peer */
    Bitfield       block_bitmap; /* what blocks the peer have */

    int            tx_bytes;
    int            tx_time;
    double         avg_tx_rate;
    int            pending_blocks;
};

struct _WingufileGetblockV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getblock_v2_proc_get_type ();

int wingufile_getblock_v2_proc_get_block (WingufileGetblockV2Proc *proc,
                                     int block_idx);

gboolean wingufile_getblock_v2_proc_is_ready (WingufileGetblockV2Proc *proc);

#endif
