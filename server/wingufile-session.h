/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_SESSION_H
#define WINGUFILE_SESSION_H

#include <ccnet.h>
#include <ccnet/cevent.h>
#include <ccnet/job-mgr.h>

#include "block-mgr.h"
#include "fs-mgr.h"
#include "commit-mgr.h"
#include "branch-mgr.h"
#include "repo-mgr.h"
#include "db.h"
#include "winguf-db.h"

#include "chunkserv-mgr.h"
#include "share-mgr.h"
#include "token-mgr.h"
#include "web-accesstoken-mgr.h"
#include "passwd-mgr.h"
#include "quota-mgr.h"
#include "listen-mgr.h"
#include "size-sched.h"

#include "mq-mgr.h"

#include <wingurpc-client.h>

struct _CcnetClient;

typedef struct _WingufileSession WingufileSession;


struct _WingufileSession {
    struct _CcnetClient *session;

    SearpcClient        *ccnetrpc_client;
    SearpcClient        *ccnetrpc_client_t;
    /* Use async rpc client on server. */
    SearpcClient        *async_ccnetrpc_client;
    SearpcClient        *async_ccnetrpc_client_t;

    /* Used in threads. */
    CcnetClientPool     *client_pool;

    char                *winguf_dir;
    char                *tmp_file_dir;
    /* Config that can be changed in run-time. */
    sqlite3             *config_db;
    /* Config that's only loaded on start */
    GKeyFile            *config;
    SeafDB              *db;

    SeafBlockManager    *block_mgr;
    SeafFSManager       *fs_mgr;
    SeafCommitManager   *commit_mgr;
    SeafBranchManager   *branch_mgr;
    SeafRepoManager     *repo_mgr;
    SeafCSManager       *cs_mgr;
    SeafShareManager	*share_mgr;
    SeafTokenManager    *token_mgr;
    SeafPasswdManager   *passwd_mgr;
    SeafQuotaManager    *quota_mgr;
    SeafListenManager   *listen_mgr;
    
    SeafWebAccessTokenManager	*web_at_mgr;

    SeafMqManager       *mq_mgr;

    CEventManager       *ev_mgr;
    CcnetJobManager     *job_mgr;

    SizeScheduler       *size_sched;

    int                  is_master;
    const char          *monitor_id;

    int                  cloud_mode;
    int                  keep_history_days;

    int                  rpc_thread_pool_size;
    int                  sync_thread_pool_size;
};

extern WingufileSession *winguf;

WingufileSession *
wingufile_session_new(const char *wingufile_dir,
                    struct _CcnetClient *ccnet_session);
int
wingufile_session_init (WingufileSession *session);

int
wingufile_session_start (WingufileSession *session);

char *
wingufile_session_get_tmp_file_path (WingufileSession *session,
                                   const char *basename,
                                   char path[]);

int
wingufile_session_set_monitor (WingufileSession *session, const char *peer_id);

#endif /* WINGUFILE_H */
