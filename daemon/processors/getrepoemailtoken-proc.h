/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_GETREPOEMAILTOKEN_PROC_H
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC                  (wingufile_getrepoemailtoken_proc_get_type ())
#define WINGUFILE_GETREPOEMAILTOKEN_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, WingufileGetrepoemailtokenProc))
#define WINGUFILE_IS_GETREPOEMAILTOKEN_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC))
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, WingufileGetrepoemailtokenProcClass))
#define IS_WINGUFILE_GETREPOEMAILTOKEN_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC))
#define WINGUFILE_GETREPOEMAILTOKEN_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_GETREPOEMAILTOKEN_PROC, WingufileGetrepoemailtokenProcClass))

typedef struct _WingufileGetrepoemailtokenProc WingufileGetrepoemailtokenProc;
typedef struct _WingufileGetrepoemailtokenProcClass WingufileGetrepoemailtokenProcClass;

struct _WingufileGetrepoemailtokenProc {
    CcnetProcessor parent_instance;
};

struct _WingufileGetrepoemailtokenProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_getrepoemailtoken_proc_get_type ();

#endif

