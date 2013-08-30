/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVCOMMIT_V2_PROC_H
#define WINGUFILE_RECVCOMMIT_V2_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_RECVCOMMIT_V2_PROC                  (wingufile_recvcommit_v2_proc_get_type ())
#define WINGUFILE_RECVCOMMIT_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVCOMMIT_V2_PROC, SeafileRecvcommitV2Proc))
#define WINGUFILE_IS_RECVCOMMIT_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVCOMMIT_V2_PROC))
#define WINGUFILE_RECVCOMMIT_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVCOMMIT_V2_PROC, SeafileRecvcommitV2ProcClass))
#define IS_WINGUFILE_RECVCOMMIT_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVCOMMIT_V2_PROC))
#define WINGUFILE_RECVCOMMIT_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVCOMMIT_V2_PROC, SeafileRecvcommitV2ProcClass))

typedef struct _SeafileRecvcommitV2Proc SeafileRecvcommitV2Proc;
typedef struct _SeafileRecvcommitV2ProcClass SeafileRecvcommitV2ProcClass;

struct _SeafileRecvcommitV2Proc {
    CcnetProcessor parent_instance;
};

struct _SeafileRecvcommitV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvcommit_v2_proc_get_type ();

#endif
