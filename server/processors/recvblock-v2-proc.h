/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVBLOCK_V2_PROC_H
#define WINGUFILE_RECVBLOCK_V2_PROC_H

#include <glib-object.h>

#define WINGUFILE_TYPE_RECVBLOCK_V2_PROC                  (wingufile_recvblock_v2_proc_get_type ())
#define WINGUFILE_RECVBLOCK_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVBLOCK_V2_PROC, WingufileRecvblockV2Proc))
#define WINGUFILE_IS_RECVBLOCK_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVBLOCK_V2_PROC))
#define WINGUFILE_RECVBLOCK_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVBLOCK_V2_PROC, WingufileRecvblockV2ProcClass))
#define IS_WINGUFILE_RECVBLOCK_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVBLOCK_V2_PROC))
#define WINGUFILE_RECVBLOCK_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVBLOCK_V2_PROC, WingufileRecvblockV2ProcClass))

typedef struct _WingufileRecvblockV2Proc WingufileRecvblockV2Proc;
typedef struct _WingufileRecvblockV2ProcClass WingufileRecvblockV2ProcClass;

struct _WingufileRecvblockV2Proc {
    CcnetProcessor parent_instance;
};

struct _WingufileRecvblockV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvblock_v2_proc_get_type ();

#endif
