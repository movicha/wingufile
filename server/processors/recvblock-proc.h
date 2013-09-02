/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVBLOCK_PROC_H
#define WINGUFILE_RECVBLOCK_PROC_H

#include <glib-object.h>

#define WINGUFILE_TYPE_RECVBLOCK_PROC                  (wingufile_recvblock_proc_get_type ())
#define WINGUFILE_RECVBLOCK_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVBLOCK_PROC, WingufileRecvblockProc))
#define WINGUFILE_IS_RECVBLOCK_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVBLOCK_PROC))
#define WINGUFILE_RECVBLOCK_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVBLOCK_PROC, WingufileRecvblockProcClass))
#define IS_WINGUFILE_RECVBLOCK_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVBLOCK_PROC))
#define WINGUFILE_RECVBLOCK_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVBLOCK_PROC, WingufileRecvblockProcClass))

typedef struct _WingufileRecvblockProc WingufileRecvblockProc;
typedef struct _WingufileRecvblockProcClass WingufileRecvblockProcClass;

struct _WingufileRecvblockProc {
    CcnetProcessor parent_instance;
};

struct _WingufileRecvblockProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvblock_proc_get_type ();

#endif
