/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTBLOCK_V2_PROC_H
#define WINGUFILE_PUTBLOCK_V2_PROC_H

#include <glib-object.h>
#include <ccnet/timer.h>

#define WINGUFILE_TYPE_PUTBLOCK_V2_PROC                  (wingufile_putblock_v2_proc_get_type ())
#define WINGUFILE_PUTBLOCK_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTBLOCK_V2_PROC, WingufilePutblockV2Proc))
#define WINGUFILE_IS_PUTBLOCK_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTBLOCK_V2_PROC))
#define WINGUFILE_PUTBLOCK_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTBLOCK_V2_PROC, WingufilePutblockV2ProcClass))
#define IS_WINGUFILE_PUTBLOCK_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTBLOCK_V2_PROC))
#define WINGUFILE_PUTBLOCK_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTBLOCK_V2_PROC, WingufilePutblockV2ProcClass))

typedef struct _WingufilePutblockV2Proc WingufilePutblockV2Proc;
typedef struct _WingufilePutblockV2ProcClass WingufilePutblockV2ProcClass;

struct _WingufilePutblockV2Proc {
    CcnetProcessor parent_instance;
};

struct _WingufilePutblockV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putblock_v2_proc_get_type ();

#endif
