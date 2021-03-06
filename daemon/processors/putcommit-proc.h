/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTCOMMIT_PROC_H
#define WINGUFILE_PUTCOMMIT_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTCOMMIT_PROC                  (wingufile_putcommit_proc_get_type ())
#define WINGUFILE_PUTCOMMIT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTCOMMIT_PROC, WingufilePutcommitProc))
#define WINGUFILE_IS_PUTCOMMIT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTCOMMIT_PROC))
#define WINGUFILE_PUTCOMMIT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTCOMMIT_PROC, WingufilePutcommitProcClass))
#define IS_WINGUFILE_PUTCOMMIT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTCOMMIT_PROC))
#define WINGUFILE_PUTCOMMIT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTCOMMIT_PROC, WingufilePutcommitProcClass))

typedef struct _WingufilePutcommitProc WingufilePutcommitProc;
typedef struct _WingufilePutcommitProcClass WingufilePutcommitProcClass;

struct _WingufilePutcommitProc {
    CcnetProcessor parent_instance;
};

struct _WingufilePutcommitProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putcommit_proc_get_type ();

#endif
