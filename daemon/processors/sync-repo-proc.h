/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SYNC_REPO_PROC_H
#define WINGUFILE_SYNC_REPO_PROC_H

#include <glib-object.h>
#include <ccnet.h>

#define WINGUFILE_TYPE_SYNC_REPO_PROC                  (wingufile_sync_repo_proc_get_type ())
#define WINGUFILE_SYNC_REPO_PROC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SYNC_REPO_PROC, WingufileSyncRepoProc))
#define WINGUFILE_IS_SYNC_REPO_PROC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SYNC_REPO_PROC))
#define WINGUFILE_SYNC_REPO_PROC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SYNC_REPO_PROC, WingufileSyncRepoProcClass))
#define IS_WINGUFILE_SYNC_REPO_PROC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SYNC_REPO_PROC))
#define WINGUFILE_SYNC_REPO_PROC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SYNC_REPO_PROC, WingufileSyncRepoProcClass))

typedef struct _WingufileSyncRepoProc WingufileSyncRepoProc;
typedef struct _WingufileSyncRepoProcClass WingufileSyncRepoProcClass;

struct _SyncTask;

struct _WingufileSyncRepoProc {
    CcnetProcessor parent_instance;

    struct _SyncTask *task;
};

struct _WingufileSyncRepoProcClass {
    CcnetProcessorClass parent_class;
};

GType wingufile_sync_repo_proc_get_type ();

void
wingufile_sync_repo_proc_set_repo (WingufileSyncRepoProc *processor,
                                 char *repo_id);

#endif
