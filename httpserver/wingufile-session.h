#ifndef WINGUFILE_SESSION_H
#define WINGUFILE_SESSION_H

#include <stdint.h>
#include <glib.h>

#include "block-mgr.h"
#include "fs-mgr.h"
#include "commit-mgr.h"
#include "branch-mgr.h"
#include "repo-mgr.h"
#include "db.h"
#include "winguf-db.h"

struct _CcnetClient;

typedef struct _WingufileSession WingufileSession;

struct CcnetClientPool;

struct _WingufileSession {
    struct _CcnetClient *session;

    char                *winguf_dir;
    char                *tmp_file_dir;
    /* Config that's only loaded on start */
    GKeyFile            *config;
    SeafDB              *db;
    char                *windows_encoding;
    gint64               max_upload_size;
    gint64               max_download_dir_size;
    char                *http_temp_dir;

    struct CcnetClientPool     *client_pool;

    SeafBlockManager    *block_mgr;
    SeafFSManager       *fs_mgr;
    SeafCommitManager   *commit_mgr;
    SeafBranchManager   *branch_mgr;
    SeafRepoManager     *repo_mgr;
};

extern WingufileSession *winguf;

WingufileSession *
wingufile_session_new (const char *wingufile_dir,
                     struct _CcnetClient *ccnet);

int
wingufile_session_init (WingufileSession *session);

int
wingufile_session_start (WingufileSession *session);

#endif
