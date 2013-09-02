/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SEAF_REPO_MGR_H
#define SEAF_REPO_MGR_H

#include "common.h"

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
#define REPO_LOCAL_HEAD       "local-head"
#define REPO_PROP_EMAIL       "email"
#define REPO_PROP_TOKEN       "token"
#define REPO_PROP_RELAY_ADDR  "relay-address"
#define REPO_PROP_RELAY_PORT  "relay-port"
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

    gchar      *relay_id;
    gchar      *worktree;
    gboolean    wt_changed;
    int         wt_check_time;
    int         last_sync_time;

    gchar      *passwd;         /* if the repo is encrypted */
    unsigned char enc_key[16];   /* 128-bit encryption key */
    unsigned char enc_iv[16];

    gchar      *email;          /* email of the user on the relay */
    gchar      *token;          /* token for access this repo on server */

    pthread_mutex_t lock;

    gboolean      worktree_invalid; /* true if worktree moved or deleted */
    gboolean      index_corrupted;

    unsigned int  auto_sync : 1;
    unsigned int  net_browsable : 1;
    unsigned int  quota_full_notified : 1;
    unsigned int  access_denied_notified : 1;
};


gboolean is_repo_id_valid (const char *id);

SeafRepo* 
winguf_repo_new (const char *id, const char *name, const char *desc);

void
winguf_repo_free (SeafRepo *repo);

int
winguf_repo_set_head (SeafRepo *repo, SeafBranch *branch);

int
winguf_repo_checkdir (SeafRepo *repo);

/* Update repo name, desc, magic etc from commit.
 */
void
winguf_repo_from_commit (SeafRepo *repo, SeafCommit *commit);

/* Update repo-related fields to commit. 
 */
void
winguf_repo_to_commit (SeafRepo *repo, SeafCommit *commit);

/*
 * Returns a list of all commits belongs to the repo.
 * The commits in the repos are all unique.
 */
GList *
winguf_repo_get_commits (SeafRepo *repo);

int
winguf_repo_verify_passwd (const char *repo_id,
                         const char *passwd,
                         const char *magic);

int
winguf_repo_index_add (SeafRepo *repo, const char *path);

int
winguf_repo_index_worktree_files (const char *repo_id,
                                const char *worktree,
                                const char *passwd,
                                char *root_id);

int
winguf_repo_index_rm (SeafRepo *repo, const char *path);

char *
winguf_repo_status (SeafRepo *repo);

gboolean
winguf_repo_is_worktree_changed (SeafRepo *repo);

gboolean
winguf_repo_is_index_unmerged (SeafRepo *repo);

char *
winguf_repo_index_commit (SeafRepo *repo, const char *desc, GError **error);

int
winguf_repo_checkout (SeafRepo *repo, const char *worktree_parent, char **error);

int
winguf_repo_reset (SeafRepo *repo, const char *commit_id, char **error);

int
winguf_repo_revert (SeafRepo *repo, const char *commit_id, char **error);

int
winguf_repo_checkout_commit (SeafRepo *repo, SeafCommit *commit, gboolean recover_merge,
                           char **error);

int
winguf_repo_merge (SeafRepo *repo, const char *branch, char **error,
                 gboolean *real_merge);

void
winguf_repo_generate_magic (SeafRepo *repo, const char *passwd);

GList *
winguf_repo_diff (SeafRepo *repo, const char *arg1, const char *arg2, char **error);

typedef struct _SeafRepoManager SeafRepoManager;
typedef struct _SeafRepoManagerPriv SeafRepoManagerPriv;

struct _SeafRepoManager {
    struct _SeafileSession *winguf;

    char *index_dir;

    SeafRepoManagerPriv *priv;
};

SeafRepoManager* 
winguf_repo_manager_new (struct _SeafileSession *winguf);

int
winguf_repo_manager_init (SeafRepoManager *mgr);

int
winguf_repo_manager_start (SeafRepoManager *mgr);

int
winguf_repo_manager_add_repo (SeafRepoManager *mgr, SeafRepo *repo);

int
winguf_repo_manager_mark_repo_deleted (SeafRepoManager *mgr, SeafRepo *repo);

int
winguf_repo_manager_del_repo (SeafRepoManager *mgr, SeafRepo *repo);

SeafRepo* 
winguf_repo_manager_create_new_repo (SeafRepoManager *mgr,
                                   const char *name,
                                   const char *desc);

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

#define MAX_REPO_TOKEN 64
#define DEFAULT_REPO_TOKEN "default"


int
winguf_repo_manager_set_repo_token (SeafRepoManager *manager, 
                                  SeafRepo *repo,
                                  const char *token);

int
winguf_repo_manager_set_repo_email (SeafRepoManager *manager, 
                                  SeafRepo *repo,
                                  const char *email);

int
winguf_repo_manager_set_repo_relay_info (SeafRepoManager *manager, 
                                       const char *repo_id,
                                       const char *relay_addr,
                                       const char *relay_port);
void
winguf_repo_manager_get_repo_relay_info (SeafRepoManager *mgr,
                                       const char *repo_id,
                                       char **relay_addr,
                                       char **relay_port);

int
winguf_repo_manager_branch_repo_unmap (SeafRepoManager *manager, SeafBranch *branch);

char *
winguf_repo_manager_get_repo_lantoken (SeafRepoManager *manager,
                                     const char *repo_id);
int
winguf_repo_manager_set_repo_lantoken (SeafRepoManager *manager,
                                     const char *repo_id,
                                     const char *token);
int
winguf_repo_manager_verify_repo_lantoken (SeafRepoManager *manager,
                                        const char *repo_id,
                                        const char *token);

char *
winguf_repo_manager_generate_tmp_token (SeafRepoManager *manager,
                                      const char *repo_id,
                                      const char *peer_id);

int
winguf_repo_manager_verify_tmp_token (SeafRepoManager *manager,
                                    const char *repo_id,
                                    const char *peer_id,
                                    const char *token);

int
winguf_repo_manager_set_repo_property (SeafRepoManager *manager,
                                     const char *repo_id,
                                     const char *key,
                                     const char *value);

char *
winguf_repo_manager_get_repo_property (SeafRepoManager *manager,
                                     const char *repo_id,
                                     const char *key);

void
winguf_repo_mamager_del_repo_property (SeafRepoManager *manager, SeafRepo *repo);

int
winguf_repo_check_worktree (SeafRepo *repo);

int
winguf_repo_manager_set_repo_worktree (SeafRepoManager *mgr,
                                     SeafRepo *repo,
                                     const char *worktree);

void
winguf_repo_manager_invalidate_repo_worktree (SeafRepoManager *mgr,
                                            SeafRepo *repo);

void
winguf_repo_manager_validate_repo_worktree (SeafRepoManager *mgr,
                                          SeafRepo *repo);

int
winguf_repo_manager_set_repo_passwd (SeafRepoManager *manager,
                                   SeafRepo *repo,
                                   const char *passwd);

int
winguf_repo_manager_set_repo_relay_id (SeafRepoManager *mgr,
                                     SeafRepo *repo,
                                     const char *relay_id);

int
winguf_repo_manager_set_merge (SeafRepoManager *manager,
                             const char *repo_id,
                             const char *remote_head);

int
winguf_repo_manager_clear_merge (SeafRepoManager *manager,
                               const char *repo_id);

typedef struct {
    gboolean in_merge;
    char remote_head[41];
} SeafRepoMergeInfo;

int
winguf_repo_manager_get_merge_info (SeafRepoManager *manager,
                                  const char *repo_id,
                                  SeafRepoMergeInfo *info);
typedef struct {
    char repo_id[41];
    char worktree[SEAF_PATH_MAX];
    int total_files;
    int finished_files;
    gboolean success;
} CheckoutTask;

typedef void (*CheckoutDoneCallback) (CheckoutTask *, SeafRepo *, void *);

int
winguf_repo_manager_add_checkout_task (SeafRepoManager *mgr,
                                     SeafRepo *repo,
                                     const char *worktree,
                                     CheckoutDoneCallback done_cb,
                                     void *cb_data);

/* Remove all the files in the worktree and then checkout again.
 * Can be used to re-checkout if wrong password was given.
 */
int
winguf_repo_manager_add_recheckout_task (SeafRepoManager *mgr,
                                       SeafRepo *repo,
                                       CheckoutDoneCallback done_cb,
                                       void *cb_data);

CheckoutTask *
winguf_repo_manager_get_checkout_task (SeafRepoManager *mgr,
                                     const char *repo_id);
int
winguf_repo_manager_update_repo_relay_info (SeafRepoManager *mgr,
                                          SeafRepo *repo,
                                          const char *new_addr,
                                          const char *new_port);

GList *
winguf_repo_load_ignore_files (const char *worktree);

gboolean
winguf_repo_check_ignore_file (GList *ignore_list, const char *fullpath);

void
winguf_repo_free_ignore_files (GList *ignore_list);

#endif
