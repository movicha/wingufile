/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_PUTBLOCK_PROC_H
#define WINGUFILE_PUTBLOCK_PROC_H

#include <glib-object.h>
#include <ccnet/timer.h>

#define WINGUFILE_TYPE_PUTBLOCK_PROC                  (wingufile_putblock_proc_get_type ())
#define WINGUFILE_PUTBLOCK_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_PUTBLOCK_PROC, SeafilePutblockProc))
#define WINGUFILE_IS_PUTBLOCK_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_PUTBLOCK_PROC))
#define WINGUFILE_PUTBLOCK_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_PUTBLOCK_PROC, SeafilePutblockProcClass))
#define IS_WINGUFILE_PUTBLOCK_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_PUTBLOCK_PROC))
#define WINGUFILE_PUTBLOCK_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_PUTBLOCK_PROC, SeafilePutblockProcClass))

typedef struct _SeafilePutblockProc SeafilePutblockProc;
typedef struct _SeafilePutblockProcClass SeafilePutblockProcClass;

struct _SeafilePutblockProc {
    CcnetProcessor parent_instance;
};

struct _SeafilePutblockProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_putblock_proc_get_type ();

#endif
