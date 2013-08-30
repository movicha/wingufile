/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETREPOEMAILTOKEN_PROC_H
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC                  (wingufile_getrepoemailtoken_proc_get_type ())
#define WINGUFILE_GETREPOEMAILTOKEN_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, SeafileGetrepoemailtokenProc))
#define WINGUFILE_IS_GETREPOEMAILTOKEN_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC))
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, SeafileGetrepoemailtokenProcClass))
#define IS_WINGUFILE_GETREPOEMAILTOKEN_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC))
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, SeafileGetrepoemailtokenProcClass))

typedef struct _SeafileGetrepoemailtokenProc SeafileGetrepoemailtokenProc;
typedef struct _SeafileGetrepoemailtokenProcClass SeafileGetrepoemailtokenProcClass;

struct _SeafileGetrepoemailtokenProc {
    CcnetProcessor parent_instance;
};

struct _SeafileGetrepoemailtokenProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getrepoemailtoken_proc_get_type ();

#endif

