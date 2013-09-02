/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SYNC_REPO_SLAVE_PROC_H
#define WINGUFILE_SYNC_REPO_SLAVE_PROC_H

#include <glib-object.h>
#include <ccnet.h>

#define WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC                  (wingufile_sync_repo_slave_proc_get_type ())
#define WINGUFILE_SYNC_REPO_SLAVE_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC, WingufileSynRepoSlaveProc))
#define WINGUFILE_IS_SYNC_REPO_SLAVE_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC))
#define WINGUFILE_SYNC_REPO_SLAVE_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC, WingufileSynRepoSlaveProcClass))
#define IS_WINGUFILE_SYNC_REPO_SLAVE_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC))
#define WINGUFILE_SYNC_REPO_SLAVE_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC, WingufileSynRepoSlaveProcClass))

typedef struct _WingufileSynRepoSlaveProc WingufileSynRepoSlaveProc;
typedef struct _WingufileSynRepoSlaveProcClass WingufileSynRepoSlaveProcClass;

struct _WingufileSynRepoSlaveProc {
    CcnetProcessor parent_instance;
};

struct _WingufileSynRepoSlaveProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sync_repo_slave_proc_get_type ();

#endif
