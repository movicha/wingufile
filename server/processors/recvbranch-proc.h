/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVBRANCH_PROC_H
#define WINGUFILE_RECVBRANCH_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_RECVBRANCH_PROC                  (wingufile_recvbranch_proc_get_type ())
#define WINGUFILE_RECVBRANCH_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVBRANCH_PROC, WingufileRecvbranchProc))
#define WINGUFILE_IS_RECVBRANCH_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVBRANCH_PROC))
#define WINGUFILE_RECVBRANCH_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVBRANCH_PROC, WingufileRecvbranchProcClass))
#define IS_WINGUFILE_RECVBRANCH_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVBRANCH_PROC))
#define WINGUFILE_RECVBRANCH_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVBRANCH_PROC, WingufileRecvbranchProcClass))

typedef struct _WingufileRecvbranchProc WingufileRecvbranchProc;
typedef struct _WingufileRecvbranchProcClass WingufileRecvbranchProcClass;

struct _WingufileRecvbranchProc {
    CcnetProcessor parent_instance;
};

struct _WingufileRecvbranchProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvbranch_proc_get_type ();

#endif

