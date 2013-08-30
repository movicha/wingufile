/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVCOMMIT_PROC_H
#define WINGUFILE_RECVCOMMIT_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_RECVCOMMIT_PROC                  (wingufile_recvcommit_proc_get_type ())
#define WINGUFILE_RECVCOMMIT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVCOMMIT_PROC, SeafileRecvcommitProc))
#define WINGUFILE_IS_RECVCOMMIT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVCOMMIT_PROC))
#define WINGUFILE_RECVCOMMIT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVCOMMIT_PROC, SeafileRecvcommitProcClass))
#define IS_WINGUFILE_RECVCOMMIT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVCOMMIT_PROC))
#define WINGUFILE_RECVCOMMIT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVCOMMIT_PROC, SeafileRecvcommitProcClass))

typedef struct _SeafileRecvcommitProc SeafileRecvcommitProc;
typedef struct _SeafileRecvcommitProcClass SeafileRecvcommitProcClass;

struct _SeafileRecvcommitProc {
    CcnetProcessor parent_instance;
};

struct _SeafileRecvcommitProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvcommit_proc_get_type ();

#endif
