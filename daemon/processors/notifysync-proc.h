/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_NOTIFYSYNC_PROC_H
#define WINGUFILE_NOTIFYSYNC_PROC_H

#include <glib-object.h>


#define WINGUFILE_TYPE_NOTIFYSYNC_PROC                  (wingufile_notifysync_proc_get_type ())
#define WINGUFILE_NOTIFYSYNC_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_NOTIFYSYNC_PROC, WingufileNotifysyncProc))
#define WINGUFILE_IS_NOTIFYSYNC_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_NOTIFYSYNC_PROC))
#define WINGUFILE_NOTIFYSYNC_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_NOTIFYSYNC_PROC, WingufileNotifysyncProcClass))
#define IS_WINGUFILE_NOTIFYSYNC_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_NOTIFYSYNC_PROC))
#define WINGUFILE_NOTIFYSYNC_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_NOTIFYSYNC_PROC, WingufileNotifysyncProcClass))

typedef struct _WingufileNotifysyncProc WingufileNotifysyncProc;
typedef struct _WingufileNotifysyncProcClass WingufileNotifysyncProcClass;

struct _WingufileNotifysyncProc {
    CcnetProcessor parent_instance;
};

struct _WingufileNotifysyncProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_notifysync_proc_get_type ();

#endif

