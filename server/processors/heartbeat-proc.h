/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_HEARTBEAT_PROC_H
#define WINGUFILE_HEARTBEAT_PROC_H

#include <glib-object.h>
#include <ccnet/processor.h>

#define WINGUFILE_TYPE_HEARTBEAT_PROC                  (wingufile_heartbeat_proc_get_type ())
#define WINGUFILE_HEARTBEAT_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_HEARTBEAT_PROC, WingufileHeartbeatProc))
#define WINGUFILE_IS_HEARTBEAT_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_HEARTBEAT_PROC))
#define WINGUFILE_HEARTBEAT_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_HEARTBEAT_PROC, WingufileHeartbeatProcClass))
#define IS_WINGUFILE_HEARTBEAT_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_HEARTBEAT_PROC))
#define WINGUFILE_HEARTBEAT_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_HEARTBEAT_PROC, WingufileHeartbeatProcClass))

typedef struct _WingufileHeartbeatProc WingufileHeartbeatProc;
typedef struct _WingufileHeartbeatProcClass WingufileHeartbeatProcClass;

struct _WingufileHeartbeatProc {
    CcnetProcessor parent_instance;
};

struct _WingufileHeartbeatProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_heartbeat_proc_get_type ();

#endif

