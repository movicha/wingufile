/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"
#include <glib/gstdio.h>

#include <ccnet.h>
#include "utils.h"
#include "avl/avl.h"
#include "log.h"

#include "wingufile-session.h"
#include "wingufile-config.h"
#include "commit-mgr.h"
#include "branch-mgr.h"
#include "repo-mgr.h"
#include "fs-mgr.h"
#include "wingufile-error.h"

#include "winguf-db.h"

#define INDEX_DIR "index"

struct _SeafRepoManagerPriv {
    avl_tree_t *repo_tree;
    pthread_rwlock_t lock;
};

static SeafRepo *
load_repo (SeafRepoManager *manager, const char *repo_id);

gboolean
is_repo_id_valid (const char *id)
{
    if (!id)
        return FALSE;

    return is_uuid_valid (id);
}

SeafRepo*
winguf_repo_new (const char *id, const char *name, const char *desc)
{
    SeafRepo* repo;

    /* valid check */
  
    
    repo = g_new0 (SeafRepo, 1);
    memcpy (repo->id, id, 36);
    repo->id[36] = '\0';

    repo->name = g_strdup(name);
    repo->desc = g_strdup(desc);

    repo->ref_cnt = 1;

    return repo;
}

void
winguf_repo_free (SeafRepo *repo)
{
    if (repo->name) g_free (repo->name);
    if (repo->desc) g_free (repo->desc);
    if (repo->category) g_free (repo->category);
    if (repo->head) winguf_branch_unref (repo->head);
    g_free (repo);
}

void
winguf_repo_ref (SeafRepo *repo)
{
    g_atomic_int_inc (&repo->ref_cnt);
}

void
winguf_repo_unref (SeafRepo *repo)
{
    if (!repo)
        return;

    if (g_atomic_int_dec_and_test (&repo->ref_cnt))
        winguf_repo_free (repo);
}

static void
set_head_common (SeafRepo *repo, SeafBranch *branch)
{
    if (repo->head)
        winguf_branch_unref (repo->head);
    repo->head = branch;
    winguf_branch_ref(branch);
}

void
winguf_repo_from_commit (SeafRepo *repo, SeafCommit *commit)
{
    repo->name = g_strdup (commit->repo_name);
    repo->desc = g_strdup (commit->repo_desc);
    repo->encrypted = commit->encrypted;
    if (repo->encrypted) {
        repo->enc_version = commit->enc_version;
        if (repo->enc_version >= 1)
            memcpy (repo->magic, commit->magic, 33);
    }
    repo->no_local_history = commit->no_local_history;
}

void
winguf_repo_to_commit (SeafRepo *repo, SeafCommit *commit)
{
    commit->repo_name = g_strdup (repo->name);
    commit->repo_desc = g_strdup (repo->desc);
    commit->encrypted = repo->encrypted;
    if (commit->encrypted) {
        commit->enc_version = repo->enc_version;
        if (commit->enc_version >= 1)
            commit->magic = g_strdup (repo->magic);
    }
    commit->no_local_history = repo->no_local_history;
}

static gboolean
collect_commit (SeafCommit *commit, void *vlist, gboolean *stop)
{
    GList **commits = vlist;

    /* The traverse function will unref the commit, so we need to ref it.
     */
    winguf_commit_ref (commit);
    *commits = g_list_prepend (*commits, commit);
    return TRUE;
}

GList *
winguf_repo_get_commits (SeafRepo *repo)
{
    GList *branches;
    GList *ptr;
    SeafBranch *branch;
    GList *commits = NULL;

    branches = winguf_branch_manager_get_branch_list (winguf->branch_mgr, repo->id);
    if (branches == NULL) {
        g_warning ("Failed to get branch list of repo %s.\n", repo->id);
        return NULL;
    }

    for (ptr = branches; ptr != NULL; ptr = ptr->next) {
        branch = ptr->data;
        gboolean res = winguf_commit_manager_traverse_commit_tree (winguf->commit_mgr,
                                                                 branch->commit_id,
                                                                 collect_commit,
                                                                 &commits,
                                                                 FALSE);
        if (!res) {
            for (ptr = commits; ptr != NULL; ptr = ptr->next)
                winguf_commit_unref ((SeafCommit *)(ptr->data));
            g_list_free (commits);
            goto out;
        }
    }

    commits = g_list_reverse (commits);

out:
    for (ptr = branches; ptr != NULL; ptr = ptr->next) {
        winguf_branch_unref ((SeafBranch *)ptr->data);
    }
    return commits;
}

static int 
compare_repo (const SeafRepo *srepo, const SeafRepo *trepo)
{
    return g_strcmp0 (srepo->id, trepo->id);
}

SeafRepoManager*
winguf_repo_manager_new (WingufileSession *winguf)
{
    SeafRepoManager *mgr = g_new0 (SeafRepoManager, 1);

    mgr->priv = g_new0 (SeafRepoManagerPriv, 1);
    mgr->winguf = winguf;

    mgr->priv->repo_tree = avl_alloc_tree ((avl_compare_t)compare_repo,
                                           NULL);

    pthread_rwlock_init (&mgr->priv->lock, NULL);

    return mgr;
}

int
winguf_repo_manager_init (SeafRepoManager *mgr)
{
    return 0;
}

int
winguf_repo_manager_start (SeafRepoManager *mgr)
{
    return 0;
}

static gboolean
repo_exists_in_db (SeafDB *db, const char *id)
{
    char sql[256];
    gboolean db_err = FALSE;

    snprintf (sql, sizeof(sql), "SELECT repo_id FROM Repo WHERE repo_id = '%s'",
              id);
    return winguf_db_check_for_existence (db, sql, &db_err);
}

SeafRepo*
winguf_repo_manager_get_repo (SeafRepoManager *manager, const gchar *id)
{
    SeafRepo repo;
    int len = strlen(id);

    if (len >= 37)
        return NULL;

    memcpy (repo.id, id, len + 1);
#if 0
    if (pthread_rwlock_rdlock (&manager->priv->lock) < 0) {
        g_warning ("[repo mgr] failed to lock repo cache.\n");
        return NULL;
    }

    avl_node_t *res = avl_search (manager->priv->repo_tree, &repo);

    pthread_rwlock_unlock (&manager->priv->lock);

    if (res) {
        winguf_repo_ref ((SeafRepo *)(res->item));
        return res->item;
    }
#endif

    if (repo_exists_in_db (manager->winguf->db, id)) {
        SeafRepo *ret = load_repo (manager, id);
        if (!ret)
            return NULL;
        /* winguf_repo_ref (ret); */
        return ret;
    }

    return NULL;
}

SeafRepo*
winguf_repo_manager_get_repo_prefix (SeafRepoManager *manager, const gchar *id)
{
    avl_node_t *node;
    SeafRepo repo, *result;
    int len = strlen(id);

    if (len >= 37)
        return NULL;

    memcpy (repo.id, id, len + 1);

    avl_search_closest (manager->priv->repo_tree, &repo, &node);
    if (node != NULL) {
        result = node->item;
        if (strncmp (id, result->id, len) == 0)
            return node->item;
    }
    return NULL;
}

gboolean
winguf_repo_manager_repo_exists (SeafRepoManager *manager, const gchar *id)
{
    SeafRepo repo;
    memcpy (repo.id, id, 37);

#if 0
    if (pthread_rwlock_rdlock (&manager->priv->lock) < 0) {
        g_warning ("[repo mgr] failed to lock repo cache.\n");
        return FALSE;
    }

    avl_node_t *res = avl_search (manager->priv->repo_tree, &repo);

    pthread_rwlock_unlock (&manager->priv->lock);

    if (res)
        return TRUE;
#endif

    return repo_exists_in_db (manager->winguf->db, id);
}

gboolean
winguf_repo_manager_repo_exists_prefix (SeafRepoManager *manager, const gchar *id)
{
    avl_node_t *node;
    SeafRepo repo;

    memcpy (repo.id, id, 37);

    avl_search_closest (manager->priv->repo_tree, &repo, &node);
    if (node != NULL)
        return TRUE;
    return FALSE;
}

static void
load_repo_commit (SeafRepoManager *manager,
                  SeafRepo *repo,
                  SeafBranch *branch)
{
    SeafCommit *commit;

    commit = winguf_commit_manager_get_commit (manager->winguf->commit_mgr,
                                             branch->commit_id);
    if (!commit) {
        g_warning ("Commit %s is missing\n", branch->commit_id);
        repo->is_corrupted = TRUE;
        return;
    }

    set_head_common (repo, branch);
    winguf_repo_from_commit (repo, commit);

    winguf_commit_unref (commit);
}

static SeafRepo *
load_repo (SeafRepoManager *manager, const char *repo_id)
{
    SeafRepo *repo;
    SeafBranch *branch;

    repo = winguf_repo_new(repo_id, NULL, NULL);
    if (!repo) {
        g_warning ("[repo mgr] failed to alloc repo.\n");
        return NULL;
    }

    repo->manager = manager;

    branch = winguf_branch_manager_get_branch (winguf->branch_mgr, repo_id, "master");
    if (!branch) {
        g_warning ("Failed to get master branch of repo %.8s.\n", repo_id);
        repo->is_corrupted = TRUE;
    } else {
        load_repo_commit (manager, repo, branch);
        winguf_branch_unref (branch);
    }

    if (repo->is_corrupted) {
        g_warning ("Repo %.8s is corrupted.\n", repo->id);
        winguf_repo_free (repo);
        return NULL;
    }

#if 0
    if (pthread_rwlock_wrlock (&manager->priv->lock) < 0) {
        g_warning ("[repo mgr] failed to lock repo cache.\n");
        winguf_repo_free (repo);
        return NULL;
    }
    avl_insert (manager->priv->repo_tree, repo);
    /* Don't need to increase ref count, since the ref count of
     * a new repo object is already 1.
     */
    pthread_rwlock_unlock (&manager->priv->lock);
#endif

    return repo;
}

static gboolean
collect_repo_id (SeafDBRow *row, void *data)
{
    GList **p_ids = data;
    const char *repo_id;

    repo_id = winguf_db_row_get_column_text (row, 0);
    *p_ids = g_list_prepend (*p_ids, g_strdup(repo_id));

    return TRUE;
}

GList *
winguf_repo_manager_get_repo_id_list (SeafRepoManager *mgr)
{
    GList *ret = NULL;
    char sql[256];

    snprintf (sql, 256, "SELECT repo_id FROM Repo");

    if (winguf_db_foreach_selected_row (mgr->winguf->db, sql, 
                                      collect_repo_id, &ret) < 0)
        return NULL;

    return ret;
}

GList *
winguf_repo_manager_get_repo_list (SeafRepoManager *mgr, int start, int limit)
{
    GList *id_list = NULL, *ptr;
    GList *ret = NULL;
    SeafRepo *repo;
    char sql[256];

    if (start == -1 && limit == -1)
        snprintf (sql, 256, "SELECT repo_id FROM Repo");
    else
        snprintf (sql, 256, "SELECT repo_id FROM Repo LIMIT %d, %d", start, limit);

    if (winguf_db_foreach_selected_row (mgr->winguf->db, sql, 
                                      collect_repo_id, &id_list) < 0)
        return NULL;

    for (ptr = id_list; ptr; ptr = ptr->next) {
        char *repo_id = ptr->data;
        repo = winguf_repo_manager_get_repo (mgr, repo_id);
        if (repo != NULL)
            ret = g_list_prepend (ret, repo);
    }

    string_list_free (id_list);
    return g_list_reverse (ret);
}
