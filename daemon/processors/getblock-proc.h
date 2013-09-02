/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETBLOCK_PROC_H
#define WINGUFILE_GETBLOCK_PROC_H

#include <glib-object.h>
#include "transfer-mgr.h"

#define WINGUFILE_TYPE_GETBLOCK_PROC                  (wingufile_getblock_proc_get_type ())
#define WINGUFILE_GETBLOCK_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETBLOCK_PROC, WingufileGetblockProc))
#define WINGUFILE_IS_GETBLOCK_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETBLOCK_PROC))
#define WINGUFILE_GETBLOCK_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETBLOCK_PROC, WingufileGetblockProcClass))
#define IS_WINGUFILE_GETBLOCK_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETBLOCK_PROC))
#define WINGUFILE_GETBLOCK_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETBLOCK_PROC, WingufileGetblockProcClass))

typedef struct _WingufileGetblockProc WingufileGetblockProc;
typedef struct _WingufileGetblockProcClass WingufileGetblockProcClass;

struct _WingufileGetblockProc {
    CcnetProcessor parent_instance;

    TransferTask  *tx_task;
    Bitfield       active;       /* what blocks to download from the peer */
    Bitfield       block_bitmap; /* what blocks the peer have */

    int            tx_bytes;
    int            tx_time;
    double         avg_tx_rate;
    int            pending_blocks;
};

struct _WingufileGetblockProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getblock_proc_get_type ();

int wingufile_getblock_proc_get_block (WingufileGetblockProc *proc,
                                     int block_idx);

gboolean wingufile_getblock_proc_is_ready (WingufileGetblockProc *proc);

#endif
