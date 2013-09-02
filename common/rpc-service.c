/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"
#include <glib/gstdio.h>
#include <ctype.h>

#include <sys/stat.h>
#include <dirent.h>
#include <ccnet.h>
#include "utils.h"

#include "wingufile-session.h"
#include "fs-mgr.h"
#include "repo-mgr.h"
#include "wingufile-error.h"
#include "wingufile-rpc.h"
#include "wingufile-config.h"

#ifdef WINGUFILE_SERVER
#include "monitor-rpc-wrappers.h"
#include "web-accesstoken-mgr.h"
#endif

#ifndef WINGUFILE_SERVER
#include "gc.h"
#endif

#include "log.h"

#ifndef WINGUFILE_SERVER
#include "../daemon/vc-utils.h"

#endif  /* WINGUFILE_SERVER */


/* -------- Utilities -------- */
static GList *
convert_repo_list (GList *inner_repos)
{
    GList *ret = NULL, *ptr;

    for (ptr = inner_repos; ptr; ptr=ptr->next) {
        SeafRepo *r = ptr->data;
#ifndef WINGUFILE_SERVER
        /* Don't display repos without worktree. */
        if (r->head == NULL || r->worktree_invalid)
            continue;
#endif

        SeafileRepo *repo = wingufile_repo_new ();
        g_object_set (repo, "id", r->id, "name", r->name,
                      "desc", r->desc, "encrypted", r->encrypted,
                      NULL);

#ifndef WINGUFILE_SERVER
    g_object_set (repo, "worktree-changed", r->wt_changed,
                  "worktree-checktime", r->wt_check_time,
                  "worktree-invalid", r->worktree_invalid,
                  "last-sync-time", r->last_sync_time,
                  "index-corrupted", r->index_corrupted,
                  NULL);

    g_object_set (repo, "worktree", r->worktree,
                  /* "auto-sync", r->auto_sync, */
                  "head_branch", r->head ? r->head->name : NULL,
                  "relay-id", r->relay_id,
                  "auto-sync", r->auto_sync,
                  NULL);

    g_object_set (repo, "passwd", r->passwd, NULL);

    g_object_set (repo,
                  "last-modify", wingufile_repo_last_modify(r->id, NULL),
                  NULL);

    g_object_set (repo, "no-local-history", r->no_local_history, NULL);
#endif

        ret = g_list_prepend (ret, repo);
    }
    ret = g_list_reverse (ret);

    return ret;
}

GList *
wingufile_list_dir_by_path(const char *commit_id, const char *path, GError **error)
{
    if (!commit_id || !path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Args can't be NULL");
        return NULL;
    }

    SeafCommit *commit;
    commit = winguf_commit_manager_get_commit(winguf->commit_mgr, commit_id);

    if (!commit) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_COMMIT, "No such commit");
        return NULL;
    }

    char *p = g_strdup(path);
    int len = strlen(p);

    /* strip trailing backslash */
    while (len > 0 && p[len-1] == '/') {
        p[len-1] = '\0';
        len--;
    }

    SeafDir *dir;
    SeafDirent *dent;
    SeafileDirent *d;

    GList *ptr;
    GList *res = NULL;

    dir = winguf_fs_manager_get_wingufdir_by_path (winguf->fs_mgr, commit->root_id,
                                               p, error);
    if (!dir) {
        winguf_warning ("Can't find winguf dir for %s\n", path);
        goto out;
    }

    for (ptr = dir->entries; ptr != NULL; ptr = ptr->next) {
        dent = ptr->data;
        d = g_object_new (WINGUFILE_TYPE_DIRENT,
                          "obj_id", dent->id,
                          "obj_name", dent->name,
                          "mode", dent->mode,
                          NULL);
        res = g_list_prepend (res, d);
    }

    winguf_dir_free (dir);
    res = g_list_reverse (res);

 out:

    g_free (p);
    winguf_commit_unref (commit);
    return res;
}

char *
wingufile_get_dirid_by_path(const char *commit_id, const char *path, GError **error)
{
    char *res = NULL;
    if (!commit_id || !path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Args can't be NULL");
        return NULL;
    }

    SeafCommit *commit;
    commit = winguf_commit_manager_get_commit(winguf->commit_mgr, commit_id);

    if (!commit) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_COMMIT, "No such commit");
        return NULL;
    }

    char *p = g_strdup(path);
    int len = strlen(p);

    /* strip trailing backslash */
    while (len > 0 && p[len-1] == '/') {
        p[len-1] = '\0';
        len--;
    }

    SeafDir *dir;
    dir = winguf_fs_manager_get_wingufdir_by_path (winguf->fs_mgr, commit->root_id,
                                               p, error);
    if (!dir) {
        winguf_warning ("Can't find winguf dir for %s\n", path);
        goto out;
    }

    res = g_strdup (dir->dir_id);
    winguf_dir_free (dir);

 out:

    g_free (p);
    winguf_commit_unref (commit);
    return res;
}

/*
 * RPC functions only available for clients.
 */

#ifndef WINGUFILE_SERVER

#include "sync-mgr.h"

GObject *
wingufile_get_session_info (GError **error)
{
    SeafileSessionInfo *info;

    info = wingufile_session_info_new ();
    g_object_set (info, "datadir", winguf->winguf_dir, NULL);
    return (GObject *) info;
}

int
wingufile_set_config (const char *key, const char *value, GError **error)
{
    return wingufile_session_config_set_string(winguf, key, value);
}

char *
wingufile_get_config (const char *key, GError **error)
{
    return wingufile_session_config_get_string(winguf, key);
}

int
wingufile_set_config_int (const char *key, int value, GError **error)
{
    return wingufile_session_config_set_int(winguf, key, value);
}

int
wingufile_get_config_int (const char *key, GError **error)
{
    gboolean exists = TRUE;

    int ret = wingufile_session_config_get_int(winguf, key, &exists);

    if (!exists) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Config not exists");
        return -1;
    }

    return ret;
}

int
wingufile_set_upload_rate_limit (int limit, GError **error)
{
    if (limit < 0)
        limit = 0;

    winguf->transfer_mgr->upload_limit = limit;

    return wingufile_session_config_set_int (winguf, KEY_UPLOAD_LIMIT, limit);
}

int
wingufile_set_download_rate_limit (int limit, GError **error)
{
    if (limit < 0)
        limit = 0;

    winguf->transfer_mgr->download_limit = limit;

    return wingufile_session_config_set_int (winguf, KEY_DOWNLOAD_LIMIT, limit);
}

int
wingufile_repo_last_modify(const char *repo_id, GError **error)
{
    SeafRepo *repo;
    SeafCommit *c;
    char *commit_id;
    int ctime = 0;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "No such repository");
        return -1;
    }

    if (!repo->head) {
        SeafBranch *branch =
            winguf_branch_manager_get_branch (winguf->branch_mgr,
                                            repo->id, "master");
        if (branch != NULL) {
            commit_id = g_strdup (branch->commit_id);
            winguf_branch_unref (branch);
        } else {
            g_warning ("[repo-mgr] Failed to get repo %s branch master\n",
                       repo_id);
            g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO,
                         "No head and branch master");
            return -1;
        }
    } else {
        commit_id = g_strdup (repo->head->commit_id);
    }

    c = winguf_commit_manager_get_commit (winguf->commit_mgr, commit_id);
    g_free (commit_id);
    if (!c)
        return -1;

    ctime = c->ctime;
    winguf_commit_unref (c);
    return ctime;
}

GObject *
wingufile_get_checkout_task (const char *repo_id, GError **error)
{
    if (!repo_id) {
        winguf_warning ("Invalid args\n");
        return NULL;
    }

    CheckoutTask *task;
    task = winguf_repo_manager_get_checkout_task(winguf->repo_mgr,
                                               repo_id);
    if (!task)
        return NULL;

    SeafileCheckoutTask *c_task = g_object_new
        (WINGUFILE_TYPE_CHECKOUT_TASK,
         "repo_id", task->repo_id,
         "worktree", task->worktree,
         "total_files", task->total_files,
         "finished_files", task->finished_files,
         NULL);

    return (GObject *)c_task;
}

char *
wingufile_gen_default_worktree (const char *worktree_parent,
                              const char *repo_name,
                              GError **error)
{
    if (!worktree_parent || !repo_name) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Empty args");
        return NULL;
    }

    return winguf_clone_manager_gen_default_worktree (winguf->clone_mgr,
                                                    worktree_parent,
                                                    repo_name);
}

char *
wingufile_clone (const char *repo_id,
               const char *relay_id,
               const char *repo_name,
               const char *worktree,
               const char *token,
               const char *passwd,
               const char *magic,
               const char *peer_addr,
               const char *peer_port,
               const char *email,
               GError **error)
{
    if (!repo_id || strlen(repo_id) != 36) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    if (!relay_id || strlen(relay_id) != 40) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid peer id");
        return NULL;
    }

    if (!worktree) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Worktre must be specified");
        return NULL;
    }

    if (!token || !peer_addr || !peer_port || !email ) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Argument can't be NULL");
        return NULL;
    }

    return winguf_clone_manager_add_task (winguf->clone_mgr,
                                        repo_id, relay_id,
                                        repo_name, token,
                                        passwd, magic, worktree,
                                        peer_addr, peer_port,
                                        email, error);
}

char *
wingufile_download (const char *repo_id,
                  const char *relay_id,
                  const char *repo_name,
                  const char *wt_parent,
                  const char *token,
                  const char *passwd,
                  const char *magic,
                  const char *peer_addr,
                  const char *peer_port,
                  const char *email,
                  GError **error)
{
    if (!repo_id || strlen(repo_id) != 36) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    if (!relay_id || strlen(relay_id) != 40) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid peer id");
        return NULL;
    }

    if (!wt_parent) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Worktre must be specified");
        return NULL;
    }

    if (!token || !peer_addr || !peer_port || !email ) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Argument can't be NULL");
        return NULL;
    }

    return winguf_clone_manager_add_download_task (winguf->clone_mgr,
                                                 repo_id, relay_id,
                                                 repo_name, token,
                                                 passwd, magic, wt_parent,
                                                 peer_addr, peer_port,
                                                 email, error);
}

int
wingufile_cancel_clone_task (const char *repo_id, GError **error)
{
    return winguf_clone_manager_cancel_task (winguf->clone_mgr, repo_id);
}

int
wingufile_remove_clone_task (const char *repo_id, GError **error)
{
    return winguf_clone_manager_remove_task (winguf->clone_mgr, repo_id);
}

GList *
wingufile_get_clone_tasks (GError **error)
{
    GList *tasks, *ptr;
    GList *ret = NULL;
    CloneTask *task;
    SeafileCloneTask *t;

    tasks = winguf_clone_manager_get_tasks (winguf->clone_mgr);
    for (ptr = tasks; ptr != NULL; ptr = ptr->next) {
        task = ptr->data;
        t = g_object_new (WINGUFILE_TYPE_CLONE_TASK,
                          "state", clone_task_state_to_str(task->state),
                          "error_str", clone_task_error_to_str(task->error),
                          "repo_id", task->repo_id,
                          "peer_id", task->peer_id,
                          "repo_name", task->repo_name,
                          "worktree", task->worktree,
                          "tx_id", task->tx_id,
                          NULL);
        ret = g_list_prepend (ret, t);
    }

    g_list_free (tasks);
    return ret;
}

int
wingufile_sync (const char *repo_id, const char *peer_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Repo ID should not be null");
        return -1;
    }

    return winguf_sync_manager_add_sync_task (winguf->sync_mgr, repo_id, peer_id,
                                            NULL, FALSE, error);
}

static void get_task_size(TransferTask *task, gint64 *rsize, gint64 *dsize)
{
    if (task->runtime_state == TASK_RT_STATE_INIT
        || task->runtime_state == TASK_RT_STATE_COMMIT
        || task->runtime_state == TASK_RT_STATE_FS
        || task->runtime_state == TASK_RT_STATE_FINISHED) {
        *rsize = task->rsize;
        *dsize = task->dsize;
    }
    if (task->runtime_state == TASK_RT_STATE_DATA) {
        if (task->type == TASK_TYPE_DOWNLOAD) {
            *dsize = task->block_list->n_valid_blocks;
            *rsize = task->block_list->n_blocks - *dsize;
        } else {
            *dsize = task->n_uploaded;
            *rsize = task->block_list->n_blocks - *dsize;
        }
    }
}

static SeafileTask *
convert_task (TransferTask *task)
{
    gint64 rsize = 0, dsize = 0;
    SeafileTask *t = wingufile_task_new();

    get_task_size (task, &rsize, &dsize);

    g_assert (strlen(task->repo_id) == 36);
    g_object_set (t, "tx_id", task->tx_id,
                  "repo_id", task->repo_id,
                  "dest_id", task->dest_id,
                  "from_branch", task->from_branch,
                  "to_branch", task->to_branch,
                  "state", task_state_to_str(task->state),
                  "rt_state", task_rt_state_to_str(task->runtime_state),
                  "rsize", rsize, "dsize", dsize,
                  "error_str", task_error_str(task->error),
                  NULL);

    if (task->type == TASK_TYPE_DOWNLOAD) {
        g_object_set (t, "ttype", "download", NULL);
        if (task->runtime_state == TASK_RT_STATE_DATA) {
            g_object_set (t, "block_total", task->block_list->n_blocks,
                          "block_done", transfer_task_get_done_blocks (task),
                          NULL);
            g_object_set (t, "rate", transfer_task_get_rate(task), NULL);
        }
    } else {
        g_object_set (t, "ttype", "upload", NULL);
        if (task->runtime_state == TASK_RT_STATE_DATA) {
            g_object_set (t, "block_total", task->block_list->n_blocks,
                          "block_done", transfer_task_get_done_blocks (task),
                          NULL);
            g_object_set (t, "rate", transfer_task_get_rate(task), NULL);
        }
    }

    return t;
}


GObject *
wingufile_find_transfer_task (const char *repo_id, GError *error)
{
    TransferTask *task;

    task = winguf_transfer_manager_find_transfer_by_repo (
        winguf->transfer_mgr, repo_id);
    if (!task)
        return NULL;

    return (GObject *)convert_task (task);
}


GObject *
wingufile_get_repo_sync_info (const char *repo_id, GError **error)
{
    SyncInfo *info;

    info = winguf_sync_manager_get_sync_info (winguf->sync_mgr, repo_id);
    if (!info)
        return NULL;

    SeafileSyncInfo *sinfo;
    sinfo = g_object_new (WINGUFILE_TYPE_SYNC_INFO,
                          "repo_id", info->repo_id,
                          "head_commit", info->head_commit,
                          "deleted_on_relay", info->deleted_on_relay,
                          "need_fetch", info->need_fetch,
                          "need_upload", info->need_upload,
                          "need_merge", info->need_merge,
                          /* "last_sync_time", info->last_sync_time,  */
                          NULL);

    return (GObject *)sinfo;
}


GObject *
wingufile_get_repo_sync_task (const char *repo_id, GError **error)
{
    SyncInfo *info = winguf_sync_manager_get_sync_info (winguf->sync_mgr, repo_id);
    if (!info || !info->current_task)
        return NULL;

    SyncTask *task = info->current_task;

    SeafileSyncTask *s_task;
    s_task = g_object_new (WINGUFILE_TYPE_SYNC_TASK,
                           "is_sync_lan", task->is_sync_lan,
                           "force_upload", task->force_upload,
                           "state", sync_state_to_str(task->state),
                           "error", sync_error_to_str(task->error),
                           "tx_id", task->tx_id,
                           "dest_id", task->dest_id,
                           "repo_id", info->repo_id,
                           NULL);

    return (GObject *)s_task;
}

GList *
wingufile_get_sync_task_list (GError **error)
{
    GHashTable *sync_info_tbl = winguf->sync_mgr->sync_infos;
    GHashTableIter iter;
    SeafileSyncTask *s_task;
    GList *task_list = NULL;
    gpointer key, value;

    g_hash_table_iter_init (&iter, sync_info_tbl);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        SyncInfo *info = value;
        if (!info->in_sync)
            continue;
        SyncTask *task = info->current_task;
        if (!task)
            continue;
        s_task = g_object_new (WINGUFILE_TYPE_SYNC_TASK,
                               "is_sync_lan", task->is_sync_lan,
                               "force_upload", task->force_upload,
                               "state", sync_state_to_str(task->state),
                               "error", sync_error_to_str(task->error),
                               "dest_id", task->dest_id,
                               "repo_id", info->repo_id,
                               "tx_id", task->tx_id,
                               NULL);
        task_list = g_list_prepend (task_list, s_task);
    }

    return task_list;
}


int
wingufile_set_repo_property (const char *repo_id,
                           const char *key,
                           const char *value,
                           GError **error)
{
    int ret;

    if (repo_id == NULL || key == NULL || value == NULL) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    SeafRepo *repo;
    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "Can't find Repo %s", repo_id);
        return -1;
    }

    ret = winguf_repo_manager_set_repo_property (winguf->repo_mgr,
                                               repo->id, key, value);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL,
                     "Failed to set key for repo %s", repo_id);
        return -1;
    }

    return 0;
}

gchar *
wingufile_get_repo_property (const char *repo_id,
                           const char *key,
                           GError **error)
{
    char *value = NULL;

    if (!repo_id || !key) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    SeafRepo *repo;
    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "Can't find Repo %s", repo_id);
        return NULL;
    }

    value = winguf_repo_manager_get_repo_property (winguf->repo_mgr, repo->id, key);
    return value;
}

char *
wingufile_get_repo_relay_address (const char *repo_id,
                                GError **error)
{
    char *relay_addr = NULL;
    
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    winguf_repo_manager_get_repo_relay_info (winguf->repo_mgr, repo_id,
                                           &relay_addr, NULL);

    return relay_addr;
}

char *
wingufile_get_repo_relay_port (const char *repo_id,
                             GError **error)
{
    char *relay_port = NULL;
    
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    winguf_repo_manager_get_repo_relay_info (winguf->repo_mgr, repo_id,
                                           NULL, &relay_port);

    return relay_port;
}

int
wingufile_update_repo_relay_info (const char *repo_id,
                                const char *new_addr,
                                const char *new_port,
                                GError **error)
{
    if (!repo_id || !new_addr || !new_port) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    int port = atoi(new_port);
    if (port <= 0 || port > 65535) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid port");
        return -1;
    }

    SeafRepo *repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        return -1;
    }

    CcnetPeer *relay = ccnet_get_peer (winguf->ccnetrpc_client, repo->relay_id);
    if (!relay) {
        GString *buf = g_string_new(NULL);
        g_string_append_printf (buf, "add-relay --id %s --addr %s:%s",
                                repo->relay_id, new_addr, new_port);

        ccnet_send_command (winguf->session, buf->str, NULL, NULL);
        g_string_free (buf, TRUE);
    } else {
        if (g_strcmp0(relay->public_addr, new_addr) != 0 ||
            relay->public_port != (uint16_t)port) {
            ccnet_update_peer_address (winguf->ccnetrpc_client, repo->relay_id,
                                       new_addr, port);
        }

        g_object_unref (relay);
    }

    return winguf_repo_manager_update_repo_relay_info (winguf->repo_mgr, repo,
                                                     new_addr, new_port);
}

int
wingufile_calc_dir_size (const char *path, GError **error)
{
    if (!path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    gint64 size_64 = ccnet_calc_directory_size(path, error);
    if (size_64 < 0) {
        winguf_warning ("failed to calculate dir size for %s\n", path);
        return -1;
    }

    /* get the size in MB */
    int size = (int) (size_64 >> 20);
    return size;
}

int
wingufile_disable_auto_sync (GError **error)
{
    return winguf_sync_manager_disable_auto_sync (winguf->sync_mgr);
}

int
wingufile_enable_auto_sync (GError **error)
{
    return winguf_sync_manager_enable_auto_sync (winguf->sync_mgr);
}

int wingufile_is_auto_sync_enabled (GError **error)
{
    return winguf_sync_manager_is_auto_sync_enabled (winguf->sync_mgr);
}


#endif  /* not define WINGUFILE_SERVER */

/*
 * RPC functions available for both clients and server.
 */

GList *
wingufile_list_dir (const char *dir_id, int offset, int limit, GError **error)
{
    SeafDir *dir;
    SeafDirent *dent;
    SeafileDirent *d;
    GList *res = NULL;
    GList *p;

    if (dir_id == NULL) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_DIR_ID, "Bad dir id");
        return NULL;
    }
    dir = winguf_fs_manager_get_wingufdir (winguf->fs_mgr, dir_id);
    if (!dir) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_DIR_ID, "Bad dir id");
        return NULL;
    }

    if (offset < 0) {
        offset = 0;
    }

    int index = 0;
    for (p = dir->entries; p != NULL; p = p->next, index++) {
        if (index < offset) {
            continue;
        }
        
        if (limit > 0) {
            if (index >= offset + limit)
                break;
        }

        dent = p->data;
        d = g_object_new (WINGUFILE_TYPE_DIRENT,
                          "obj_id", dent->id,
                          "obj_name", dent->name,
                          "mode", dent->mode,
                          NULL);
        res = g_list_prepend (res, d);
    }

    winguf_dir_free (dir);
    res = g_list_reverse (res);
    return res;
}

GList *
wingufile_branch_gets (const char *repo_id, GError **error)
{
    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    GList *blist = winguf_branch_manager_get_branch_list(winguf->branch_mgr,
                                                       repo_id);
    GList *ptr;
    GList *ret = NULL;

    for (ptr = blist; ptr; ptr=ptr->next) {
        SeafBranch *b = ptr->data;
        SeafileBranch *branch = wingufile_branch_new ();
        g_object_set (branch, "repo_id", b->repo_id, "name", b->name,
                      "commit_id", b->commit_id, NULL);
        ret = g_list_prepend (ret, branch);
        winguf_branch_unref (b);
    }
    ret = g_list_reverse (ret);
    g_list_free (blist);
    return ret;
}

GList*
wingufile_get_repo_list (int start, int limit, GError **error)
{
    GList *repos = winguf_repo_manager_get_repo_list(winguf->repo_mgr, start, limit);
    GList *ret = NULL;

    ret = convert_repo_list (repos);

#ifdef WINGUFILE_SERVER
    GList *ptr;
    for (ptr = repos; ptr != NULL; ptr = ptr->next)
        winguf_repo_unref ((SeafRepo *)ptr->data);
#endif
    g_list_free (repos);

    return ret;
}

GObject*
wingufile_get_repo (const char *repo_id, GError **error)
{
    SeafRepo *r;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }
    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    r = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    /* Don't return repo that's not checked out. */
    if (r == NULL)
        return NULL;

#ifndef WINGUFILE_SERVER
    if (r->head == NULL || r->worktree_invalid)
        return NULL;
#endif

    SeafileRepo *repo = wingufile_repo_new ();
    g_object_set (repo, "id", r->id, "name", r->name,
                  "desc", r->desc, "encrypted", r->encrypted,
                  "magic", r->magic,
                  "head_branch", r->head ? r->head->name : NULL,
                  "head_cmmt_id", r->head ? r->head->commit_id : NULL,
                  NULL);

#ifdef WINGUFILE_SERVER
    if (r->virtual_info) {
        g_object_set (repo,
                      "is_virtual", TRUE,
                      "origin_repo_id", r->virtual_info->origin_repo_id,
                      "origin_path", r->virtual_info->path,
                      NULL);
    }
#endif

#ifndef WINGUFILE_SERVER
    g_object_set (repo, "worktree-changed", r->wt_changed,
                  "worktree-checktime", r->wt_check_time,
                  "worktree-invalid", r->worktree_invalid,
                  "last-sync-time", r->last_sync_time,
                  "index-corrupted", r->index_corrupted,
                  NULL);

    g_object_set (repo, "worktree", r->worktree,
                  "relay-id", r->relay_id,
                  "auto-sync", r->auto_sync,
                  NULL);

    g_object_set (repo, "passwd", r->passwd, NULL);

    g_object_set (repo,
                  "last-modify", wingufile_repo_last_modify(r->id, NULL),
                  NULL);

    g_object_set (repo, "no-local-history", r->no_local_history, NULL);
#endif  /* WINGUFILE_SERVER */

#ifdef WINGUFILE_SERVER
    winguf_repo_unref (r);
#endif

    return (GObject *)repo;
}

inline SeafileCommit *
convert_to_wingufile_commit (SeafCommit *c)
{
    SeafileCommit *commit = wingufile_commit_new ();
    g_object_set (commit,
                  "id", c->commit_id,
                  "creator_name", c->creator_name,
                  "creator", c->creator_id,
                  "desc", c->desc,
                  "ctime", c->ctime,
                  "repo_id", c->repo_id,
                  "root_id", c->root_id,
                  "parent_id", c->parent_id,
                  "second_parent_id", c->second_parent_id,
                  NULL);
    return commit;
}

GObject*
wingufile_get_commit (const gchar *id, GError **error)
{
    SeafileCommit *commit;
    SeafCommit *c;

    c = winguf_commit_manager_get_commit (winguf->commit_mgr, id);
    if (!c)
        return NULL;

    commit = convert_to_wingufile_commit (c);
    winguf_commit_unref (c);
    return (GObject *)commit;
}

static void
free_commit_list (GList *commits)
{
    SeafileCommit *c;
    GList *ptr;

    for (ptr = commits; ptr; ptr = ptr->next) {
        c = ptr->data;
        g_object_unref (c);
    }
    g_list_free (commits);
}

struct CollectParam {
    int offset;
    int limit;
    int count;
    GList *commits;
#ifdef WINGUFILE_SERVER
    gint64 truncate_time;
    gboolean traversed_head;
#endif
};

static gboolean
get_commit (SeafCommit *c, void *data, gboolean *stop)
{
    struct CollectParam *cp = data;

#ifdef WINGUFILE_SERVER
    if (cp->truncate_time == 0)
    {
        *stop = TRUE;
        /* Stop after traversing the head commit. */
    }
    else if (cp->truncate_time > 0 &&
             (gint64)(c->ctime) < cp->truncate_time &&
             cp->traversed_head)
    {
        *stop = TRUE;
        return TRUE;
    }

    /* Always traverse the head commit. */
    if (!cp->traversed_head)
        cp->traversed_head = TRUE;
#endif

    /* if offset = 1, limit = 1, we should stop when the count = 2 */
    if (cp->limit > 0 && cp->count >= cp->offset + cp->limit) {
        *stop = TRUE;
        return TRUE;  /* TRUE to indicate no error */
    }

    if (cp->count >= cp->offset) {
        SeafileCommit *commit = convert_to_wingufile_commit (c);
        cp->commits = g_list_prepend (cp->commits, commit);
    }

    ++cp->count;
    return TRUE;                /* TRUE to indicate no error */
}


GList*
wingufile_get_commit_list (const char *repo_id,
                         int offset,
                         int limit,
                         GError **error)
{
    SeafRepo *repo;
    GList *commits = NULL;
    gboolean ret;
    struct CollectParam cp;
    char *commit_id;

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    /* correct parameter */
    if (offset < 0)
        offset = 0;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "No such repository");
        return NULL;
    }

    if (!repo->head) {
        SeafBranch *branch =
            winguf_branch_manager_get_branch (winguf->branch_mgr,
                                            repo->id, "master");
        if (branch != NULL) {
            commit_id = g_strdup (branch->commit_id);
            winguf_branch_unref (branch);
        } else {
            g_warning ("[repo-mgr] Failed to get repo %s branch master\n",
                       repo_id);
            g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO,
                         "No head and branch master");
#ifdef WINGUFILE_SERVER
            winguf_repo_unref (repo);
#endif
            return NULL;
        }
    } else {
        commit_id = g_strdup (repo->head->commit_id);
    }

#ifdef WINGUFILE_SERVER
    winguf_repo_unref (repo);
#endif

    /* Init CollectParam */
    memset (&cp, 0, sizeof(cp));
    cp.offset = offset;
    cp.limit = limit;

#ifdef WINGUFILE_SERVER
    cp.truncate_time = winguf_repo_manager_get_repo_truncate_time (winguf->repo_mgr,
                                                                 repo_id);
#endif

    ret = winguf_commit_manager_traverse_commit_tree (
            winguf->commit_mgr, commit_id, get_commit, &cp, FALSE);
    g_free (commit_id);

    if (!ret) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_LIST_COMMITS, "Failed to list commits");
        free_commit_list (commits);
        return NULL;
    }

    commits = g_list_reverse (cp.commits);
    return commits;
}

int
wingufile_destroy_repo (const char *repo_id, GError **error)
{
    SeafRepo *repo;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }
    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such repository");
        return -1;
    }

#ifndef WINGUFILE_SERVER
    if (!winguf->started) {
        winguf_message ("System not started, skip removing repo.\n");
        return -1;
    }

    if (repo->auto_sync)
        winguf_wt_monitor_unwatch_repo (winguf->wt_monitor, repo_id);

    SyncInfo *info = winguf_sync_manager_get_sync_info (winguf->sync_mgr, repo_id);

    /* If we are syncing the repo,
     * we just mark the repo as deleted and let sync-mgr actually delete it.
     * Otherwise we are safe to delete the repo.
     */
    char *worktree = g_strdup (repo->worktree);
    if (info != NULL && info->in_sync) {
        winguf_repo_manager_mark_repo_deleted (winguf->repo_mgr, repo);
    } else {
        winguf_repo_manager_del_repo (winguf->repo_mgr, repo);
    }

    /* Publish a message, for applet to notify in the system tray */
    winguf_mq_manager_publish_notification (winguf->mq_mgr,
                                          "repo.removed",
                                          worktree);
    g_free (worktree);
#else
    winguf_repo_manager_del_repo (winguf->repo_mgr, repo->id);
    winguf_share_manager_remove_repo (winguf->share_mgr, repo->id);
    winguf_repo_unref (repo);
#endif

    return 0;
}

#ifndef WINGUFILE_SERVER
int
wingufile_gc (GError **error)
{
    return gc_start ();
}
#endif

#if 0
int
wingufile_gc_get_progress (GError **error)
{
    int progress = gc_get_progress ();

    if (progress < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GC_NOT_STARTED, "GC is not running");
        return -1;
    }

    return progress;
}
#endif

/*
 * RPC functions only available for server.
 */

#ifdef WINGUFILE_SERVER

int
wingufile_edit_repo (const char *repo_id,
                   const char *name,
                   const char *description,
                   const char *user,
                   GError **error)
{
    SeafRepo *repo = NULL;
    SeafCommit *commit = NULL, *parent = NULL;
    int ret = 0;

    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "No user given");
        return -1;
    }

    if (!name && !description) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "At least one argument should be non-null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

retry:
    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such library");
        return -1;
    }

    if (!name)
        name = repo->name;
    if (!description)
        description = repo->desc;

    /*
     * We only change repo_name or repo_desc, so just copy the head commit
     * and change these two fields.
     */
    parent = winguf_commit_manager_get_commit (winguf->commit_mgr,
                                             repo->head->commit_id);
    if (!parent) {
        winguf_warning ("Failed to get commit %s.\n", repo->head->commit_id);
        ret = -1;
        goto out;
    }

    commit = winguf_commit_new (NULL,
                              repo->id,
                              parent->root_id,
                              user,
                              EMPTY_SHA1,
                              "Changed library name or description",
                              0);
    commit->parent_id = g_strdup(parent->commit_id);
    winguf_repo_to_commit (repo, commit);

    g_free (commit->repo_name);
    commit->repo_name = g_strdup(name);
    g_free (commit->repo_desc);
    commit->repo_desc = g_strdup(description);

    if (winguf_commit_manager_add_commit (winguf->commit_mgr, commit) < 0) {
        ret = -1;
        goto out;
    }

    winguf_branch_set_commit (repo->head, commit->commit_id);
    if (winguf_branch_manager_test_and_update_branch (winguf->branch_mgr,
                                                    repo->head,
                                                    parent->commit_id) < 0) {
        winguf_repo_unref (repo);
        winguf_commit_unref (commit);
        winguf_commit_unref (parent);
        repo = NULL;
        commit = NULL;
        parent = NULL;
        goto retry;
    }

out:
    winguf_commit_unref (commit);
    winguf_commit_unref (parent);
    winguf_repo_unref (repo);

    return ret;
}

#include "diff-simple.h"

inline static const char*
get_diff_status_str(char status)
{
    if (status == DIFF_STATUS_ADDED)
        return "add";
    if (status == DIFF_STATUS_DELETED)
        return "del";
    if (status == DIFF_STATUS_MODIFIED)
        return "mod";
    if (status == DIFF_STATUS_RENAMED)
        return "mov";
    if (status == DIFF_STATUS_DIR_ADDED)
        return "newdir";
    if (status == DIFF_STATUS_DIR_DELETED)
        return "deldir";
    return NULL;
}

GList *
wingufile_diff (const char *repo_id, const char *arg1, const char *arg2, GError **error)
{
    SeafRepo *repo;
    char *err_msgs = NULL;
    GList *diff_entries, *p;
    GList *ret = NULL;

    if (!repo_id || !arg1 || !arg2) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such repository");
        return NULL;
    }

    diff_entries = winguf_repo_diff (repo, arg1, arg2, &err_msgs);
    if (err_msgs) {
        g_set_error (error, WINGUFILE_DOMAIN, -1, "%s", err_msgs);
        g_free (err_msgs);
#ifdef WINGUFILE_SERVER
        winguf_repo_unref (repo);
#endif
        return NULL;
    }

#ifdef WINGUFILE_SERVER
    winguf_repo_unref (repo);
#endif

    for (p = diff_entries; p != NULL; p = p->next) {
        DiffEntry *de = p->data;
        SeafileDiffEntry *entry = g_object_new (
            WINGUFILE_TYPE_DIFF_ENTRY,
            "status", get_diff_status_str(de->status),
            "name", de->name,
            "new_name", de->new_name,
            NULL);
        ret = g_list_prepend (ret, entry);
    }

    for (p = diff_entries; p != NULL; p = p->next) {
        DiffEntry *de = p->data;
        diff_entry_free (de);
    }
    g_list_free (diff_entries);

    return g_list_reverse (ret);
}

int
wingufile_is_repo_owner (const char *email,
                       const char *repo_id,
                       GError **error)
{
    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return 0;
    }

    char *owner = winguf_repo_manager_get_repo_owner (winguf->repo_mgr, repo_id);
    if (!owner) {
        /* g_warning ("Failed to get owner info for repo %s.\n", repo_id); */
        return 0;
    }

    if (strcmp(owner, email) != 0) {
        g_free (owner);
        return 0;
    }

    g_free (owner);
    return 1;
}

char *
wingufile_get_repo_owner (const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }
    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *owner = winguf_repo_manager_get_repo_owner (winguf->repo_mgr, repo_id);
    /* if (!owner){ */
    /*     g_warning ("Failed to get repo owner for repo %s.\n", repo_id); */
    /* } */

    return owner;
}


GList *
wingufile_list_owned_repos (const char *email, GError **error)
{
    GList *ret = NULL;
    GList *repos, *ptr;
    SeafRepo *r;
    SeafileRepo *repo;

    repos = winguf_repo_manager_get_repos_by_owner (winguf->repo_mgr, email);
    ptr = repos;
    while (ptr) {
        r = ptr->data;

        repo = wingufile_repo_new ();
        g_object_set (repo, "id", r->id, "name", r->name,
                      "desc", r->desc, "encrypted", r->encrypted,
                      "head_cmmt_id", r->head ? r->head->commit_id : NULL,
                      "is_virtual", (r->virtual_info != NULL),
                      NULL);
        ret = g_list_prepend (ret, repo);
        winguf_repo_unref (r);
        ptr = ptr->next;
    }
    g_list_free (repos);
    ret = g_list_reverse (ret);

    return ret;
}

int
wingufile_add_chunk_server (const char *server, GError **error)
{
    SeafCSManager *cs_mgr = winguf->cs_mgr;
    CcnetPeer *peer;

    peer = ccnet_get_peer_by_idname (winguf->ccnetrpc_client, server);
    if (!peer) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid peer id or name %s", server);
        return -1;
    }

    if (winguf_cs_manager_add_chunk_server (cs_mgr, peer->id) < 0) {
        g_object_unref (peer);
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL, "Failed to add chunk server %s", server);
        return -1;
    }

    g_object_unref (peer);
    return 0;
}

int
wingufile_del_chunk_server (const char *server, GError **error)
{
    SeafCSManager *cs_mgr = winguf->cs_mgr;
    CcnetPeer *peer;

    peer = ccnet_get_peer_by_idname (winguf->ccnetrpc_client, server);
    if (!peer) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid peer id or name %s", server);
        return -1;
    }

    if (winguf_cs_manager_del_chunk_server (cs_mgr, peer->id) < 0) {
        g_object_unref (peer);
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL, "Failed to delete chunk server %s", server);
        return -1;
    }

    g_object_unref (peer);
    return 0;
}

char *
wingufile_list_chunk_servers (GError **error)
{
    SeafCSManager *cs_mgr = winguf->cs_mgr;
    GList *servers, *ptr;
    char *cs_id;
    CcnetPeer *peer;
    GString *buf = g_string_new ("");

    servers = winguf_cs_manager_get_chunk_servers (cs_mgr);
    ptr = servers;
    while (ptr) {
        cs_id = ptr->data;
        peer = ccnet_get_peer (winguf->ccnetrpc_client, cs_id);
        if (!peer) {
            g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL, "Internal error");
            g_string_free (buf, TRUE);
            return NULL;
        }
        g_object_unref (peer);

        g_string_append_printf (buf, "%s\n", cs_id);
        ptr = ptr->next;
    }
    g_list_free (servers);

    return (g_string_free (buf, FALSE));
}

int
wingufile_set_monitor (const char *monitor_id, GError **error)
{
    CcnetPeer *peer;

    peer = ccnet_get_peer (winguf->ccnetrpc_client, monitor_id);
    if (!peer) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid peer id %s",
                     monitor_id);
        return -1;
    }
    g_object_unref (peer);

    if (wingufile_session_set_monitor (winguf, monitor_id) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL, "Failed to set monitor to %s",
                     monitor_id);
        return -1;
    }

    return 0;
}

char *
wingufile_get_monitor (GError **error)
{
    return g_strdup (winguf->monitor_id);
}

gint64
wingufile_get_user_quota_usage (const char *email, GError **error)
{
    gint64 ret;

    if (!email) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad user id");
        return -1;
    }

    ret = winguf_quota_manager_get_user_usage (winguf->quota_mgr, email);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

gint64
wingufile_get_user_share_usage (const char *email, GError **error)
{
    gint64 ret;

    if (!email) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad user id");
        return -1;
    }

    ret = winguf_quota_manager_get_user_share_usage (winguf->quota_mgr, email);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

gint64
wingufile_get_org_quota_usage (int org_id, GError **error)
{
    gint64 ret;

    ret = winguf_quota_manager_get_org_usage (winguf->quota_mgr, org_id);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

gint64
wingufile_get_org_user_quota_usage (int org_id, const char *user, GError **error)
{
    gint64 ret;

    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad user id");
        return -1;
    }

    ret = winguf_quota_manager_get_org_user_usage (winguf->quota_mgr, org_id, user);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

gint64
wingufile_server_repo_size(const char *repo_id, GError **error)
{
    gint64 ret;

    if (!repo_id || strlen(repo_id) != 36) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad repo id");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    ret = winguf_repo_manager_get_repo_size (winguf->repo_mgr, repo_id);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

int
wingufile_set_repo_history_limit (const char *repo_id,
                                int days,
                                GError **error)
{
    if (!repo_id || !is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_set_repo_history_limit (winguf->repo_mgr,
                                                  repo_id,
                                                  days) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL, "DB Error");
        return -1;
    }

    return 0;
}

int
wingufile_get_repo_history_limit (const char *repo_id,
                                GError **error)
{
    if (!repo_id || !is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return  winguf_repo_manager_get_repo_history_limit (winguf->repo_mgr, repo_id);
}

int
wingufile_repo_set_access_property (const char *repo_id, const char *ap, GError **error)
{
    int ret;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (strlen(repo_id) != 36) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Wrong repo id");
        return -1;
    }

    if (g_strcmp0(ap, "public") != 0 && g_strcmp0(ap, "own") != 0 && g_strcmp0(ap, "private") != 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Wrong access property");
        return -1;
    }

    ret = winguf_repo_manager_set_access_property (winguf->repo_mgr, repo_id, ap);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal server error");
        return -1;
    }

    return ret;
}

char *
wingufile_repo_query_access_property (const char *repo_id, GError **error)
{
    char *ret;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    if (strlen(repo_id) != 36) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Wrong repo id");
        return NULL;
    }

    ret = winguf_repo_manager_query_access_property (winguf->repo_mgr, repo_id);

    return ret;
}

char *
wingufile_web_get_access_token (const char *repo_id,
                              const char *obj_id,
                              const char *op,
                              const char *username,
                              GError **error)
{
    char *token;

    if (!repo_id || !obj_id || !op || !username) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Missing args");
        return NULL;
    }

    token = winguf_web_at_manager_get_access_token (winguf->web_at_mgr,
                                                  repo_id, obj_id, op, username);
    return token;
}

GObject *
wingufile_web_query_access_token (const char *token, GError **error)
{
    SeafileWebAccess *webaccess = NULL;

    if (!token) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Token should not be null");
        return NULL;
    }

    webaccess = winguf_web_at_manager_query_access_token (winguf->web_at_mgr,
                                                        token);
    if (webaccess)
        return (GObject *)webaccess;

    return NULL;
}

int
wingufile_add_share (const char *repo_id, const char *from_email,
                   const char *to_email, const char *permission, GError **error)
{
    int ret;

    if (!repo_id || !from_email || !to_email || !permission) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Missing args");
        return -1;
    }

    if (g_strcmp0 (from_email, to_email) == 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Can not share repo to myself");
        return -1;
    }

    ret = winguf_share_manager_add_share (winguf->share_mgr, repo_id, from_email,
                                        to_email, permission);

    return ret;
}

GList *
wingufile_list_share_repos (const char *email, const char *type,
                          int start, int limit, GError **error)
{
    if (g_strcmp0 (type, "from_email") != 0 &&
        g_strcmp0 (type, "to_email") != 0 ) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Wrong type argument");
        return NULL;
    }

    return winguf_share_manager_list_share_repos (winguf->share_mgr,
                                                email, type,
                                                start, limit);
}

GList *
wingufile_list_org_share_repos (int org_id, const char *email, const char *type,
                              int start, int limit, GError **error)
{
    if (g_strcmp0 (type, "from_email") != 0 &&
        g_strcmp0 (type, "to_email") != 0 ) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Wrong type argument");
        return NULL;
    }

    return winguf_share_manager_list_org_share_repos (winguf->share_mgr,
                                                    org_id, email, type,
                                                    start, limit);
}

int
wingufile_remove_share (const char *repo_id, const char *from_email,
                      const char *to_email, GError **error)
{
    int ret;

    if (!repo_id || !from_email ||!to_email) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Missing args");
        return -1;
    }

    ret = winguf_share_manager_remove_share (winguf->share_mgr, repo_id, from_email,
                                           to_email);

    return ret;
}

/* Group repo RPC. */

int
wingufile_group_share_repo (const char *repo_id, int group_id,
                          const char *user_name, const char *permission,
                          GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    int ret;

    if (group_id <= 0 || !user_name || !repo_id || !permission) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad input argument");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    ret = winguf_repo_manager_add_group_repo (mgr, repo_id, group_id, user_name,
                                            permission, error);

    return ret;
}

int
wingufile_group_unshare_repo (const char *repo_id, int group_id,
                            const char *user_name, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    int ret;

    if (!user_name || !repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "User name and repo id can not be NULL");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    ret = winguf_repo_manager_del_group_repo (mgr, repo_id, group_id, error);

    return ret;

}

char *
wingufile_get_shared_groups_by_repo(const char *repo_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *group_ids = NULL, *ptr;
    GString *result;
    
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    group_ids = winguf_repo_manager_get_groups_by_repo (mgr, repo_id, error);
    if (!group_ids) {
        return NULL;
    }

    result = g_string_new("");
    ptr = group_ids;
    while (ptr) {
        g_string_append_printf (result, "%d\n", (int)(long)ptr->data);
        ptr = ptr->next;
    }
    g_list_free (group_ids);

    return g_string_free (result, FALSE);
}

char *
wingufile_get_group_repoids (int group_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *repo_ids = NULL, *ptr;
    GString *result;

    repo_ids = winguf_repo_manager_get_group_repoids (mgr, group_id, error);
    if (!repo_ids) {
        return NULL;
    }

    result = g_string_new("");
    ptr = repo_ids;
    while (ptr) {
        g_string_append_printf (result, "%s\n", (char *)ptr->data);
        g_free (ptr->data);
        ptr = ptr->next;
    }
    g_list_free (repo_ids);

    return g_string_free (result, FALSE);
}

GList *
wingufile_get_group_repos_by_owner (char *user, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *ret = NULL;

    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "user name can not be NULL");
        return NULL;
    }

    ret = winguf_repo_manager_get_group_repos_by_owner (mgr, user, error);
    if (!ret) {
        return NULL;
    }

    return g_list_reverse (ret);
}

char *
wingufile_get_group_repo_owner (const char *repo_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GString *result = g_string_new ("");

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *share_from = winguf_repo_manager_get_group_repo_owner (mgr, repo_id,
                                                               error);
    if (share_from) {
        g_string_append_printf (result, "%s", share_from);
        g_free (share_from);
    }

    return g_string_free (result, FALSE);
}

int
wingufile_remove_repo_group(int group_id, const char *username, GError **error)
{
    if (group_id <= 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Wrong group id argument");
        return -1;
    }

    return winguf_repo_manager_remove_group_repos (winguf->repo_mgr,
                                                 group_id, username,
                                                 error);
}

/* Inner public repo RPC */

int
wingufile_set_inner_pub_repo (const char *repo_id,
                            const char *permission,
                            GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_set_inner_pub_repo (winguf->repo_mgr,
                                              repo_id, permission) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal error");
        return -1;
    }

    return 0;
}

int
wingufile_unset_inner_pub_repo (const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_unset_inner_pub_repo (winguf->repo_mgr, repo_id) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal error");
        return -1;
    }

    return 0;
}

GList *
wingufile_list_inner_pub_repos (GError **error)
{
    return winguf_repo_manager_list_inner_pub_repos (winguf->repo_mgr);
}

gint64
wingufile_count_inner_pub_repos (GError **error)
{
    return winguf_repo_manager_count_inner_pub_repos (winguf->repo_mgr);
}

GList *
wingufile_list_inner_pub_repos_by_owner (const char *user, GError **error)
{
    if (!user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return NULL;
    }

    return winguf_repo_manager_list_inner_pub_repos_by_owner (winguf->repo_mgr, user);
}

int
wingufile_is_inner_pub_repo (const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_is_inner_pub_repo (winguf->repo_mgr, repo_id);
}

/* Org Repo RPC. */

GList *
wingufile_get_org_repo_list (int org_id, int start, int limit, GError **error)
{
    GList *repos = NULL;
    GList *ret = NULL;

    if (org_id < 0 || start < 0 || limit < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return NULL;
    }

    repos = winguf_repo_manager_get_org_repo_list (winguf->repo_mgr, org_id,
                                                 start ,limit);
    ret = convert_repo_list (repos);

    GList *ptr;
    for (ptr = repos; ptr != NULL; ptr = ptr->next)
        winguf_repo_unref ((SeafRepo *)ptr->data);

    g_list_free (repos);

    return ret;
}

int
wingufile_remove_org_repo_by_org_id (int org_id, GError **error)
{
    if (org_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    return winguf_repo_manager_remove_org_repo_by_org_id (winguf->repo_mgr, org_id);
}

/* Org Group Repo RPC. */

int
wingufile_add_org_group_repo (const char *repo_id,
                            int org_id,
                            int group_id,
                            const char *owner,
                            const char *permission,
                            GError **error)
{
    if (!repo_id || !owner || org_id < 0 || group_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_add_org_group_repo (winguf->repo_mgr,
                                                 repo_id,
                                                 org_id,
                                                 group_id,
                                                 owner,
                                                 permission,
                                                 error);
}

int
wingufile_del_org_group_repo (const char *repo_id,
                            int org_id,
                            int group_id,
                            GError **error)
{
    if (!repo_id || org_id < 0 || group_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_del_org_group_repo (winguf->repo_mgr,
                                                 repo_id,
                                                 org_id,
                                                 group_id,
                                                 error);
}

char *
wingufile_get_org_group_repoids (int org_id, int group_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *repo_ids = NULL, *ptr;
    GString *result;

    if (org_id < 0 || group_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return NULL;
    }
    
    repo_ids = winguf_repo_manager_get_org_group_repoids (mgr, org_id, group_id,
                                                        error);
    if (!repo_ids) {
        return NULL;
    }

    result = g_string_new("");
    ptr = repo_ids;
    while (ptr) {
        g_string_append_printf (result, "%s\n", (char *)ptr->data);
        g_free (ptr->data);
        ptr = ptr->next;
    }
    g_list_free (repo_ids);

    return g_string_free (result, FALSE);
}

char *
wingufile_get_org_group_repo_owner (int org_id, int group_id,
                                  const char *repo_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GString *result = g_string_new ("");

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *owner = winguf_repo_manager_get_org_group_repo_owner (mgr, org_id,
                                                              group_id,
                                                              repo_id, error);
    if (owner) {
        g_string_append_printf (result, "%s", owner);
        g_free (owner);
    }

    return g_string_free (result, FALSE);
    
}

GList *
wingufile_get_org_group_repos_by_owner (int org_id, const char *user,
                                      GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *ret = NULL;

    if (!user || org_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    ret = winguf_repo_manager_get_org_group_repos_by_owner (mgr, org_id, user,
                                                          error);
    if (!ret) {
        return NULL;
    }

    return g_list_reverse (ret);
}

char *
wingufile_get_org_groups_by_repo (int org_id, const char *repo_id,
                                GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GList *group_ids = NULL, *ptr;
    GString *result;
    
    if (!repo_id || org_id < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    group_ids = winguf_repo_manager_get_org_groups_by_repo (mgr, org_id,
                                                          repo_id, error);
    if (!group_ids) {
        return NULL;
    }

    result = g_string_new("");
    ptr = group_ids;
    while (ptr) {
        g_string_append_printf (result, "%d\n", (int)(long)ptr->data);
        ptr = ptr->next;
    }
    g_list_free (group_ids);

    return g_string_free (result, FALSE);
}

/* Org inner public repo RPC */

int
wingufile_set_org_inner_pub_repo (int org_id,
                                const char *repo_id,
                                const char *permission,
                                GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_set_org_inner_pub_repo (winguf->repo_mgr,
                                                  org_id, repo_id,
                                                  permission) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal error");
        return -1;
    }

    return 0;
}

int
wingufile_unset_org_inner_pub_repo (int org_id, const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad args");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_unset_org_inner_pub_repo (winguf->repo_mgr,
                                                    org_id, repo_id) < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL, "Internal error");
        return -1;
    }

    return 0;
}

GList *
wingufile_list_org_inner_pub_repos (int org_id, GError **error)
{
    return winguf_repo_manager_list_org_inner_pub_repos (winguf->repo_mgr, org_id);
}

GList *
wingufile_list_org_inner_pub_repos_by_owner (int org_id,
                                           const char *user,
                                           GError **error)
{
    if (!user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return NULL;
    }

    return winguf_repo_manager_list_org_inner_pub_repos_by_owner (winguf->repo_mgr,
                                                                org_id, user);
}

gint64
wingufile_get_file_size (const char *file_id, GError **error)
{
    gint64 file_size;

    if (!file_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "File id can not be NULL");
        return -1;
    }

    file_size = winguf_fs_manager_get_file_size (winguf->fs_mgr, file_id);
    if (file_size < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL,
                     "failed to read file size");
        return -1;
    }

    return file_size;
}

gint64
wingufile_get_dir_size (const char *dir_id, GError **error)
{
    gint64 dir_size;

    if (!dir_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Dir id can not be NULL");
        return -1;
    }

    dir_size = winguf_fs_manager_get_fs_size (winguf->fs_mgr, dir_id);
    if (dir_size < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Failed to caculate dir size");
        return -1;
    }

    return dir_size;
}

int
wingufile_set_passwd (const char *repo_id,
                    const char *user,
                    const char *passwd,
                    GError **error)
{
    if (!repo_id || strlen(repo_id) != 36 || !user || !passwd) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    if (winguf_passwd_manager_set_passwd (winguf->passwd_mgr,
                                        repo_id, user, passwd,
                                        error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_unset_passwd (const char *repo_id,
                      const char *user,
                      GError **error)
{
    if (!repo_id || strlen(repo_id) != 36 || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    if (winguf_passwd_manager_unset_passwd (winguf->passwd_mgr,
                                          repo_id, user,
                                          error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_is_passwd_set (const char *repo_id, const char *user, GError **error)
{
    if (!repo_id || strlen(repo_id) != 36 || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    return winguf_passwd_manager_is_passwd_set (winguf->passwd_mgr,
                                              repo_id, user);
}

GObject *
wingufile_get_decrypt_key (const char *repo_id, const char *user, GError **error)
{
    SeafileCryptKey *ret;

    if (!repo_id || strlen(repo_id) != 36 || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    ret = winguf_passwd_manager_get_decrypt_key (winguf->passwd_mgr,
                                               repo_id, user);
    if (!ret) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_GENERAL,
                     "Password was not set");
        return NULL;
    }

    return (GObject *)ret;
}

int
wingufile_revert_on_server (const char *repo_id,
                          const char *commit_id,
                          const char *user_name,
                          GError **error)
{
    if (!repo_id || strlen(repo_id) != 36 ||
        !commit_id || strlen(commit_id) != 40 ||
        !user_name) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_revert_on_server (winguf->repo_mgr,
                                               repo_id,
                                               commit_id,
                                               user_name,
                                               error);
}

int
wingufile_post_file (const char *repo_id, const char *temp_file_path,
                   const char *parent_dir, const char *file_name,
                   const char *user,
                   GError **error)
{
    if (!repo_id || !temp_file_path || !parent_dir || !file_name || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_post_file (winguf->repo_mgr, repo_id,
                                     temp_file_path, parent_dir,
                                     file_name, user,
                                     error) < 0) {
        return -1;
    }

    return 0;
}

char *
wingufile_post_multi_files (const char *repo_id,
                          const char *parent_dir,
                          const char *filenames_json,
                          const char *paths_json,
                          const char *user,
                          GError **error)
{
    if (!repo_id || !filenames_json || !parent_dir || !paths_json || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Argument should not be null");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *new_ids = NULL;
    if (winguf_repo_manager_post_multi_files (winguf->repo_mgr,
                                            repo_id,
                                            parent_dir,
                                            filenames_json,
                                            paths_json,
                                            user,
                                            &new_ids,
                                            error) < 0) {
        return NULL;
    }

    return new_ids;
}

char *
wingufile_put_file (const char *repo_id, const char *temp_file_path,
                  const char *parent_dir, const char *file_name,
                  const char *user, const char *head_id,
                  GError **error)
{
    if (!repo_id || !temp_file_path || !parent_dir || !file_name || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Argument should not be null");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *new_file_id = NULL;
    winguf_repo_manager_put_file (winguf->repo_mgr, repo_id,
                                temp_file_path, parent_dir,
                                file_name, user, head_id,
                                &new_file_id, error);
    return new_file_id;
}

int
wingufile_post_dir (const char *repo_id, const char *parent_dir,
                  const char *new_dir_name, const char *user,
                  GError **error)
{
    if (!repo_id || !parent_dir || !new_dir_name || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_post_dir (winguf->repo_mgr, repo_id,
                                    parent_dir, new_dir_name,
                                    user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_post_empty_file (const char *repo_id, const char *parent_dir,
                         const char *new_file_name, const char *user,
                         GError **error)
{
    if (!repo_id || !parent_dir || !new_file_name || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_post_empty_file (winguf->repo_mgr, repo_id,
                                           parent_dir, new_file_name,
                                           user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_del_file (const char *repo_id, const char *parent_dir,
                  const char *file_name, const char *user,
                  GError **error)
{
    if (!repo_id || !parent_dir || !file_name || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_del_file (winguf->repo_mgr, repo_id,
                                    parent_dir, file_name,
                                    user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_copy_file (const char *src_repo_id,
                   const char *src_dir,
                   const char *src_filename,
                   const char *dst_repo_id,
                   const char *dst_dir,
                   const char *dst_filename,
                   const char *user,
                   GError **error)
{
    if (!src_repo_id || !src_dir || !src_filename ||
        !dst_repo_id || !dst_dir || !dst_filename || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (src_repo_id) || !is_uuid_valid(dst_repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_copy_file (winguf->repo_mgr,
                                     src_repo_id, src_dir, src_filename,
                                     dst_repo_id, dst_dir, dst_filename,
                                     user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_move_file (const char *src_repo_id,
                   const char *src_dir,
                   const char *src_filename,
                   const char *dst_repo_id,
                   const char *dst_dir,
                   const char *dst_filename,
                   const char *user,
                   GError **error)
{
    if (!src_repo_id || !src_dir || !src_filename ||
        !dst_repo_id || !dst_dir || !dst_filename || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (src_repo_id) || !is_uuid_valid(dst_repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_move_file (winguf->repo_mgr,
                                     src_repo_id, src_dir, src_filename,
                                     dst_repo_id, dst_dir, dst_filename,
                                     user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_rename_file (const char *repo_id,
                     const char *parent_dir,
                     const char *oldname,
                     const char *newname,
                     const char *user,
                     GError **error)
{
    if (!repo_id || !parent_dir || !oldname || !newname || !user) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    if (winguf_repo_manager_rename_file (winguf->repo_mgr, repo_id,
                                       parent_dir, oldname, newname,
                                       user, error) < 0) {
        return -1;
    }

    return 0;
}

int
wingufile_is_valid_filename (const char *repo_id,
                           const char *filename,
                           GError **error)
{
    if (!repo_id || !filename) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    int ret = winguf_repo_manager_is_valid_filename (winguf->repo_mgr,
                                                   repo_id,
                                                   filename,
                                                   error);
    return ret;
}

char *
wingufile_create_repo (const char *repo_name,
                     const char *repo_desc,
                     const char *owner_email,
                     const char *passwd,
                     GError **error)
{
    if (!repo_name || !repo_desc || !owner_email) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    char *repo_id;

    repo_id = winguf_repo_manager_create_new_repo (winguf->repo_mgr,
                                                 repo_name, repo_desc,
                                                 owner_email, passwd,
                                                 error);
    return repo_id;
}

char *
wingufile_create_org_repo (const char *repo_name,
                         const char *repo_desc,
                         const char *user,
                         const char *passwd,
                         int org_id,
                         GError **error)
{
    if (!repo_name || !repo_desc || !user || org_id <= 0) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return NULL;
    }

    char *repo_id;

    repo_id = winguf_repo_manager_create_org_repo (winguf->repo_mgr,
                                                 repo_name, repo_desc,
                                                 user, passwd,
                                                 org_id, error);
    return repo_id;

}

GList *
wingufile_list_org_repos_by_owner (int org_id, const char *user, GError **error)
{
    GList *ret = NULL;
    GList *repos, *ptr;
    SeafRepo *r;
    SeafileRepo *repo;

    repos = winguf_repo_manager_get_org_repos_by_owner (winguf->repo_mgr, org_id,
                                                      user);
    ptr = repos;
    while (ptr) {
        r = ptr->data;
        repo = wingufile_repo_new ();
        g_object_set (repo, "id", r->id, "name", r->name,
                      "desc", r->desc, "encrypted", r->encrypted, NULL);
        ret = g_list_prepend (ret, repo);
        winguf_repo_unref (r);
        ptr = ptr->next;
    }
    g_list_free (repos);
    ret = g_list_reverse (ret);

    return ret;
}

char *
wingufile_get_org_repo_owner (const char *repo_id, GError **error)
{
    SeafRepoManager *mgr = winguf->repo_mgr;
    GString *result = g_string_new ("");

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *owner = winguf_repo_manager_get_org_repo_owner (mgr, repo_id);
    if (owner) {
        g_string_append_printf (result, "%s", owner);
        g_free (owner);
    }

    return g_string_free (result, FALSE);
}

int
wingufile_get_org_id_by_repo_id (const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_get_org_id_by_repo_id (winguf->repo_mgr, repo_id,
                                                    error);
}

int
wingufile_set_user_quota (const char *user, gint64 quota, GError **error)
{
    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    return winguf_quota_manager_set_user_quota (winguf->quota_mgr, user, quota);
}

gint64
wingufile_get_user_quota (const char *user, GError **error)
{
    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    return winguf_quota_manager_get_user_quota (winguf->quota_mgr, user);
}

int
wingufile_set_org_quota (int org_id, gint64 quota, GError **error)
{
    return winguf_quota_manager_set_org_quota (winguf->quota_mgr, org_id, quota);
}

gint64
wingufile_get_org_quota (int org_id, GError **error)
{
    return winguf_quota_manager_get_org_quota (winguf->quota_mgr, org_id);
}

int
wingufile_set_org_user_quota (int org_id, const char *user, gint64 quota, GError **error)
{
    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    return winguf_quota_manager_set_org_user_quota (winguf->quota_mgr,
                                                  org_id, user, quota);
}

gint64
wingufile_get_org_user_quota (int org_id, const char *user, GError **error)
{
    if (!user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    return winguf_quota_manager_get_org_user_quota (winguf->quota_mgr, org_id, user);
}

int
wingufile_check_quota (const char *repo_id, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Bad arguments");
        return -1;
    }

    return winguf_quota_manager_check_quota (winguf->quota_mgr, repo_id);
}

static char *
get_obj_id_by_path (const char *repo_id,
                    const char *path,
                    gboolean want_dir,
                    GError **error)
{
    SeafRepo *repo = NULL;
    SeafCommit *commit = NULL;
    char *obj_id = NULL;

    if (!repo_id || !path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL,
                     "Get repo error");
        goto out;
    }

    commit = winguf_commit_manager_get_commit (winguf->commit_mgr,
                                             repo->head->commit_id);
    if (!commit) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL,
                     "Get commit error");
        goto out;
    }

    guint32 mode = 0;
    obj_id = winguf_fs_manager_path_to_obj_id (winguf->fs_mgr, commit->root_id,
                                             path, &mode, error);

out:
    if (repo)
        winguf_repo_unref (repo);
    if (commit)
        winguf_commit_unref (commit);
    if (obj_id) {
        /* check if the mode matches */
        if ((want_dir && !S_ISDIR(mode)) || ((!want_dir) && S_ISDIR(mode))) {
            g_free (obj_id);
            return NULL;
        }
    }

    return obj_id;
}

char *wingufile_get_file_id_by_path (const char *repo_id,
                                   const char *path,
                                   GError **error)
{
    return get_obj_id_by_path (repo_id, path, FALSE, error);
}

char *wingufile_get_dir_id_by_path (const char *repo_id,
                                  const char *path,
                                  GError **error)
{
    return get_obj_id_by_path (repo_id, path, TRUE, error);
}

GList *
wingufile_list_file_revisions (const char *repo_id,
                             const char *path,
                             int max_revision,
                             int limit,
                             GError **error)
{
    if (!repo_id || !path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    GList *commit_list;
    commit_list = winguf_repo_manager_list_file_revisions (winguf->repo_mgr,
                                                         repo_id, path,
                                                         max_revision,
                                                         limit, error);
    GList *l = NULL;
    if (commit_list) {
        GList *p;
        for (p = commit_list; p; p = p->next) {
            SeafCommit *commit = p->data;
            SeafileCommit *c = convert_to_wingufile_commit(commit);
            l = g_list_prepend (l, c);
            winguf_commit_unref (commit);
        }
        g_list_free (commit_list);
        l = g_list_reverse (l);
    }

    return l;
}

GList *
wingufile_calc_files_last_modified (const char *repo_id,
                                  const char *parent_dir,
                                  int limit,
                                  GError **error)
{
    if (!repo_id || !parent_dir) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    return winguf_repo_manager_calc_files_last_modified (winguf->repo_mgr,
                                repo_id, parent_dir, limit, error);
}

int
wingufile_revert_file (const char *repo_id,
                     const char *commit_id,
                     const char *path,
                     const char *user,
                     GError **error)
{
    if (!repo_id || !commit_id || !path || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_revert_file (winguf->repo_mgr,
                                          repo_id, commit_id,
                                          path, user, error);
}

int
wingufile_revert_dir (const char *repo_id,
                    const char *commit_id,
                    const char *path,
                    const char *user,
                    GError **error)
{
    if (!repo_id || !commit_id || !path || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_revert_dir (winguf->repo_mgr,
                                         repo_id, commit_id,
                                         path, user, error);
}

GList *
wingufile_get_deleted (const char *repo_id, int show_days, GError **error)
{
    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Bad arguments");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    return winguf_repo_manager_get_deleted_entries (winguf->repo_mgr,
                                                  repo_id, show_days, error);
}

char *
wingufile_generate_repo_token (const char *repo_id,
                             const char *email,
                             GError **error)
{
    char *token;

    if (!repo_id || !email) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    token = winguf_repo_manager_generate_repo_token (winguf->repo_mgr, repo_id, email, error);    

    return token;
}

int
wingufile_delete_repo_token (const char *repo_id,
                           const char *token,
                           const char *user,
                           GError **error)
{
    if (!repo_id || !token || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_delete_token (winguf->repo_mgr,
                                           repo_id, token, user, error);
}

GList *
wingufile_list_repo_tokens (const char *repo_id,
                          GError **error)
{
    GList *ret_list;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    ret_list = winguf_repo_manager_list_repo_tokens (winguf->repo_mgr, repo_id, error);

    return ret_list;
}

GList *
wingufile_list_repo_tokens_by_email (const char *email,
                                   GError **error)
{
    GList *ret_list;

    if (!email) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    ret_list = winguf_repo_manager_list_repo_tokens_by_email (winguf->repo_mgr, email, error);

    return ret_list;
}


char *
wingufile_check_permission (const char *repo_id, const char *user, GError **error)
{
    if (!repo_id || !user) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    return winguf_repo_manager_check_permission (winguf->repo_mgr,
                                               repo_id, user, error);
}

int
wingufile_set_share_permission (const char *repo_id,
                              const char *from_email,
                              const char *to_email,
                              const char *permission,
                              GError **error)
{
    if (!repo_id || !from_email || !to_email || !permission) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    return winguf_share_manager_set_permission (winguf->share_mgr,
                                              repo_id,
                                              from_email,
                                              to_email,
                                              permission);
}

int
wingufile_set_group_repo_permission (int group_id,
                                   const char *repo_id,
                                   const char *permission,
                                   GError **error)
{
    if (!repo_id || !permission) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_set_group_repo_perm (winguf->repo_mgr,
                                                  repo_id,
                                                  group_id,
                                                  permission,
                                                  error);
}

int
wingufile_set_org_group_repo_permission (int org_id,
                                       int group_id,
                                       const char *repo_id,
                                       const char *permission,
                                       GError **error)
{
    if (!repo_id || !permission) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    if (!is_uuid_valid (repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return -1;
    }

    return winguf_repo_manager_set_org_group_repo_perm (winguf->repo_mgr,
                                                      repo_id,
                                                      org_id,
                                                      group_id,
                                                      permission,
                                                      error);
}

char *
wingufile_get_file_id_by_commit_and_path(const char *commit_id,
                                       const char *path,
                                       GError **error)
{
    SeafCommit *commit;
    char *file_id;

    if (!commit_id || !path) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Arguments should not be empty");
        return NULL;
    }

    commit = winguf_commit_manager_get_commit(winguf->commit_mgr, commit_id);
    if (!commit) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "bad commit id");
        return NULL;
    }

    file_id = winguf_fs_manager_path_to_obj_id (winguf->fs_mgr,
                        commit->root_id, path, NULL, error);

    winguf_commit_unref(commit);

    return file_id;
}

/* Virtual repo related */

char *
wingufile_create_virtual_repo (const char *origin_repo_id,
                             const char *path,
                             const char *repo_name,
                             const char *repo_desc,
                             const char *owner,
                             GError **error)
{
    if (!origin_repo_id || !path ||!repo_name || !repo_desc || !owner) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    if (!is_uuid_valid (origin_repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Invalid repo id");
        return NULL;
    }

    char *repo_id;

    repo_id = winguf_repo_manager_create_virtual_repo (winguf->repo_mgr,
                                                     origin_repo_id, path,
                                                     repo_name, repo_desc,
                                                     owner, error);
    return repo_id;
}

GList *
wingufile_get_virtual_repos_by_owner (const char *owner, GError **error)
{
    GList *repos, *ret = NULL, *ptr;
    SeafRepo *r, *o;
    SeafileRepo *repo;
    char *orig_repo_id;
    gboolean is_original_owner;

    if (!owner) {
        g_set_error (error, 0, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    repos = winguf_repo_manager_get_virtual_repos_by_owner (winguf->repo_mgr,
                                                          owner,
                                                          error);
    for (ptr = repos; ptr != NULL; ptr = ptr->next) {
        r = ptr->data;

        orig_repo_id = r->virtual_info->origin_repo_id;
        o = winguf_repo_manager_get_repo (winguf->repo_mgr, orig_repo_id);
        if (!o) {
            winguf_warning ("Failed to get origin repo %.10s.\n", orig_repo_id);
            winguf_repo_unref (r);
            continue;
        }

        char *orig_owner = winguf_repo_manager_get_repo_owner (winguf->repo_mgr,
                                                             orig_repo_id);
        if (g_strcmp0 (orig_owner, owner) == 0)
            is_original_owner = TRUE;
        else
            is_original_owner = FALSE;
        g_free (orig_owner);

        char *perm = winguf_repo_manager_check_permission (winguf->repo_mgr,
                                                         r->id, owner, NULL);

        repo = wingufile_repo_new ();
        g_object_set (repo,
                      "id", r->id, "name", r->name,
                      "head_cmmt_id", r->head ? r->head->commit_id : NULL,
                      "is_virtual", TRUE,
                      "origin_repo_id", r->virtual_info->origin_repo_id,
                      "origin_repo_name", o->name,
                      "origin_path", r->virtual_info->path,
                      "is_original_owner", is_original_owner,
                      "virtual_perm", perm,
                      NULL);

        ret = g_list_prepend (ret, repo);
        winguf_repo_unref (r);
        winguf_repo_unref (o);
        g_free (perm);
    }
    g_list_free (repos);

    return g_list_reverse (ret);
}

GObject *
wingufile_get_virtual_repo (const char *origin_repo,
                          const char *path,
                          const char *owner,
                          GError **error)
{
    char *repo_id;
    GObject *repo_obj;

    repo_id = winguf_repo_manager_get_virtual_repo_id (winguf->repo_mgr,
                                                     origin_repo,
                                                     path,
                                                     owner);
    if (!repo_id)
        return NULL;

    repo_obj = wingufile_get_repo (repo_id, error);

    g_free (repo_id);
    return repo_obj;
}

#endif  /* WINGUFILE_SERVER */
