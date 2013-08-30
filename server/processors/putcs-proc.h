/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTCS_PROC_H
#define WINGUFILE_PUTCS_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_PUTCS_PROC                  (wingufile_putcs_proc_get_type ())
#define WINGUFILE_PUTCS_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTCS_PROC, SeafilePutcsProc))
#define WINGUFILE_IS_PUTCS_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTCS_PROC))
#define WINGUFILE_PUTCS_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTCS_PROC, SeafilePutcsProcClass))
#define IS_WINGUFILE_PUTCS_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTCS_PROC))
#define WINGUFILE_PUTCS_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTCS_PROC, SeafilePutcsProcClass))

typedef struct _SeafilePutcsProc SeafilePutcsProc;
typedef struct _SeafilePutcsProcClass SeafilePutcsProcClass;

struct _SeafilePutcsProc {
    CcnetProcessor parent_instance;
};

struct _SeafilePutcsProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putcs_proc_get_type ();

#endif

