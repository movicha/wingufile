/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SEAF_REPO_MGR_H
#define SEAF_REPO_MGR_H

#include <pthread.h>

#include "wingufile-object.h"
#include "commit-mgr.h"
#include "branch-mgr.h"

#define REPO_AUTO_SYNC        "auto-sync"
#define REPO_AUTO_FETCH       "auto-fetch"
#define REPO_AUTO_UPLOAD      "auto-upload"
#define REPO_AUTO_MERGE       "auto-merge"
#define REPO_AUTO_COMMIT      "auto-commit"
#define REPO_RELAY_ID         "relay-id"
#define REPO_NET_BROWSABLE    "net-browsable"
#define REPO_DOUBLE_SYNC      "double-sync"
#define REPO_REMOTE_HEAD      "remote-head"
#define REPO_ENCRYPTED 0x1

struct _SeafRepoManager;
typedef struct _SeafRepo SeafRepo;

struct _SeafRepo {
    struct _SeafRepoManager *manager;

    gchar       id[37];
    gchar      *name;
    gchar      *desc;
    gchar      *category;       /* not used yet */
    gboolean    encrypted;
    int         enc_version;
    gchar       magic[33];       /* hash(repo_id + passwd), key stretched. */
    gboolean    no_local_history;

    SeafBranch *head;

    gboolean    is_corrupted;
    gboolean    delete_pending;
    int         ref_cnt;
};

gboolean is_repo_id_valid (const char *id);

SeafRepo* 
winguf_repo_new (const char *id, const char *name, const char *desc);

void
winguf_repo_free (SeafRepo *repo);

void
winguf_repo_ref (SeafRepo *repo);

void
winguf_repo_unref (SeafRepo *repo);

typedef struct _SeafRepoManager SeafRepoManager;
typedef struct _SeafRepoManagerPriv SeafRepoManagerPriv;

struct _SeafRepoManager {
    struct _WingufileSession *winguf;

    SeafRepoManagerPriv *priv;
};

SeafRepoManager* 
winguf_repo_manager_new (struct _WingufileSession *winguf);

int
winguf_repo_manager_init (SeafRepoManager *mgr);

int
winguf_repo_manager_start (SeafRepoManager *mgr);

int
winguf_repo_manager_add_repo (SeafRepoManager *mgr, SeafRepo *repo);

int
winguf_repo_manager_del_repo (SeafRepoManager *mgr, SeafRepo *repo);

SeafRepo* 
winguf_repo_manager_get_repo (SeafRepoManager *manager, const gchar *id);

SeafRepo* 
winguf_repo_manager_get_repo_prefix (SeafRepoManager *manager, const gchar *id);

gboolean
winguf_repo_manager_repo_exists (SeafRepoManager *manager, const gchar *id);

gboolean
winguf_repo_manager_repo_exists_prefix (SeafRepoManager *manager, const gchar *id);

GList* 
winguf_repo_manager_get_repo_list (SeafRepoManager *mgr, int start, int limit);

GList *
winguf_repo_manager_get_repo_id_list (SeafRepoManager *mgr);

#endif
