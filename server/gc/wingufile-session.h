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

typedef struct _SeafileSession SeafileSession;

struct _SeafileSession {
    struct _CcnetClient *session;

    char                *winguf_dir;
    char                *tmp_file_dir;
    /* Config that's only loaded on start */
    GKeyFile            *config;
    SeafDB              *db;

    SeafBlockManager    *block_mgr;
    SeafFSManager       *fs_mgr;
    SeafCommitManager   *commit_mgr;
    SeafBranchManager   *branch_mgr;
    SeafRepoManager     *repo_mgr;

    int                  keep_history_days;
};

extern SeafileSession *winguf;

SeafileSession *
wingufile_session_new (const char *wingufile_dir,
                     struct _CcnetClient *ccnet);

#endif
