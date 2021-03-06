/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTCOMMIT_V2_PROC_H
#define WINGUFILE_PUTCOMMIT_V2_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTCOMMIT_V2_PROC                  (wingufile_putcommit_v2_proc_get_type ())
#define WINGUFILE_PUTCOMMIT_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTCOMMIT_V2_PROC, WingufilePutcommitV2Proc))
#define WINGUFILE_IS_PUTCOMMIT_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTCOMMIT_V2_PROC))
#define WINGUFILE_PUTCOMMIT_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTCOMMIT_V2_PROC, WingufilePutcommitV2ProcClass))
#define IS_WINGUFILE_PUTCOMMIT_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTCOMMIT_V2_PROC))
#define WINGUFILE_PUTCOMMIT_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTCOMMIT_V2_PROC, WingufilePutcommitV2ProcClass))

typedef struct _WingufilePutcommitV2Proc WingufilePutcommitV2Proc;
typedef struct _WingufilePutcommitV2ProcClass WingufilePutcommitV2ProcClass;

struct _WingufilePutcommitV2Proc {
    CcnetProcessor parent_instance;
};

struct _WingufilePutcommitV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putcommit_v2_proc_get_type ();

#endif
