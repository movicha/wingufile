/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTCS_V2_PROC_H
#define WINGUFILE_PUTCS_V2_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_PUTCS_V2_PROC                  (wingufile_putcs_v2_proc_get_type ())
#define WINGUFILE_PUTCS_V2_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTCS_V2_PROC, WingufilePutcsV2Proc))
#define WINGUFILE_IS_PUTCS_V2_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTCS_V2_PROC))
#define WINGUFILE_PUTCS_V2_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTCS_V2_PROC, WingufilePutcsV2ProcClass))
#define IS_WINGUFILE_PUTCS_V2_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTCS_V2_PROC))
#define WINGUFILE_PUTCS_V2_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTCS_V2_PROC, WingufilePutcsV2ProcClass))

typedef struct _WingufilePutcsV2Proc WingufilePutcsV2Proc;
typedef struct _WingufilePutcsV2ProcClass WingufilePutcsV2ProcClass;

struct _WingufilePutcsV2Proc {
    CcnetProcessor parent_instance;
};

struct _WingufilePutcsV2ProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putcs_v2_proc_get_type ();

#endif

