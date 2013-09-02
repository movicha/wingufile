/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SESSION_H
#define WINGUFILE_SESSION_H

#include <glib-object.h>
#include <ccnet/cevent.h>
#include <ccnet/mqclient-proc.h>
#include <ccnet/job-mgr.h>

#include "block-mgr.h"
#include "fs-mgr.h"
#include "commit-mgr.h"
#include "branch-mgr.h"
#include "repo-mgr.h"
#include "info-mgr.h"
#include "clone-mgr.h"
#include "db.h"

#include "transfer-mgr.h"
#include "share-mgr.h"
#include "sync-mgr.h"
#include "wt-monitor.h"
#include "mq-mgr.h"
#include <wingurpc-client.h>

struct _CcnetClient;


#define WINGUFILE_TYPE_SESSION                  (wingufile_session_get_type ())
#define WINGUFILE_SESSION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), WINGUFILE_TYPE_SESSION, WingufileSession))
#define WINGUFILE_IS_SESSION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WINGUFILE_TYPE_SESSION))
#define WINGUFILE_SESSION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), WINGUFILE_TYPE_SESSION, WingufileSessionClass))
#define WINGUFILE_IS_SESSION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), WINGUFILE_TYPE_SESSION))
#define WINGUFILE_SESSION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), WINGUFILE_TYPE_SESSION, WingufileSessionClass))


typedef struct _WingufileSession WingufileSession;
typedef struct _WingufileSessionClass WingufileSessionClass;

struct _WingufileSession {
    GObject         parent_instance;

    struct _CcnetClient *session;

    SearpcClient        *ccnetrpc_client;
    SearpcClient        *appletrpc_client;

    char                *winguf_dir;
    char                *tmp_file_dir;
    char                *worktree_dir; /* the default directory for
                                        * storing worktrees  */
    sqlite3             *config_db;

    SeafBlockManager    *block_mgr;
    SeafFSManager       *fs_mgr;
    SeafCommitManager   *commit_mgr;
    SeafBranchManager   *branch_mgr;
    SeafRepoManager     *repo_mgr;
    SeafTransferManager *transfer_mgr;
    SeafCloneManager    *clone_mgr;
    SeafSyncManager     *sync_mgr;
    SeafWTMonitor       *wt_monitor;
    SeafMqManager       *mq_mgr;

    CEventManager       *ev_mgr;
    CcnetJobManager     *job_mgr;

    /* Set after all components are up and running. */
    gboolean             started;
};

struct _WingufileSessionClass
{
    GObjectClass    parent_class;
};


extern WingufileSession *winguf;

WingufileSession *
wingufile_session_new(const char *wingufile_dir,
                    const char *worktree_dir,
                    struct _CcnetClient *ccnet_session);
void
wingufile_session_prepare (WingufileSession *session);

void
wingufile_session_start (WingufileSession *session);

char *
wingufile_session_get_tmp_file_path (WingufileSession *session,
                                   const char *basename,
                                   char path[]);
#if 0
void
wingufile_session_add_event (WingufileSession *session, 
                           const char *type,
                           const char *first, ...);
#endif

#endif /* WINGUFILE_H */
