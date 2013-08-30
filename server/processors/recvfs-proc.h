/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_RECVFS_PROC_H
#define WINGUFILE_RECVFS_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_RECVFS_PROC                  (wingufile_recvfs_proc_get_type ())
#define WINGUFILE_RECVFS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_RECVFS_PROC, SeafileRecvfsProc))
#define WINGUFILE_IS_RECVFS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_RECVFS_PROC))
#define WINGUFILE_RECVFS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_RECVFS_PROC, SeafileRecvfsProcClass))
#define IS_WINGUFILE_RECVFS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_RECVFS_PROC))
#define WINGUFILE_RECVFS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_RECVFS_PROC, SeafileRecvfsProcClass))

typedef struct _SeafileRecvfsProc SeafileRecvfsProc;
typedef struct _SeafileRecvfsProcClass SeafileRecvfsProcClass;

struct _SeafileRecvfsProc {
    CcnetProcessor parent_instance;
};

struct _SeafileRecvfsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_recvfs_proc_get_type ();

#endif

