/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTBRANCH_PROC_H
#define WINGUFILE_PUTBRANCH_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_PUTBRANCH_PROC                  (wingufile_putbranch_proc_get_type ())
#define WINGUFILE_PUTBRANCH_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTBRANCH_PROC, SeafilePutbranchProc))
#define WINGUFILE_IS_PUTBRANCH_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTBRANCH_PROC))
#define WINGUFILE_PUTBRANCH_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTBRANCH_PROC, SeafilePutbranchProcClass))
#define IS_WINGUFILE_PUTBRANCH_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTBRANCH_PROC))
#define WINGUFILE_PUTBRANCH_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTBRANCH_PROC, SeafilePutbranchProcClass))

typedef struct _SeafilePutbranchProc SeafilePutbranchProc;
typedef struct _SeafilePutbranchProcClass SeafilePutbranchProcClass;

struct _SeafilePutbranchProc {
    CcnetProcessor parent_instance;
};

struct _SeafilePutbranchProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putbranch_proc_get_type ();

#endif

