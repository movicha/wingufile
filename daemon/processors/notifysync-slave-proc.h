/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_NOTIFYSYNC_SLAVE_PROC_H
#define WINGUFILE_NOTIFYSYNC_SLAVE_PROC_H

#include <glib-object.h>

#define WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC                  (wingufile_notifysync_slave_proc_get_type ())
#define WINGUFILE_NOTIFYSYNC_SLAVE_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC, WingufileNotifysyncSlaveProc))
#define WINGUFILE_IS_NOTIFYSYNC_SLAVE_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC))
#define WINGUFILE_NOTIFYSYNC_SLAVE_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC, WingufileNotifysyncSlaveProcClass))
#define WINGUFILE_IS_NOTIFYSYNC_SLAVE_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC))
#define WINGUFILE_NOTIFYSYNC_SLAVE_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_NOTIFYSYNC_SLAVE_PROC, WingufileNotifysyncSlaveProcClass))

typedef struct _WingufileNotifysyncSlaveProc WingufileNotifysyncSlaveProc;
typedef struct _WingufileNotifysyncSlaveProcClass WingufileNotifysyncSlaveProcClass;

struct _WingufileNotifysyncSlaveProc {
    CcnetProcessor parent_instance;
};

struct _WingufileNotifysyncSlaveProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_notifysync_slave_proc_get_type ();

#endif
