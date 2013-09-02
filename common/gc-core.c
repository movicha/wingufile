/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include "wingufile-session.h"
#include "bloom-filter.h"
#include "gc-core.h"

#define DEBUG_FLAG WINGUFILE_DEBUG_OTHER
#include "log.h"

#define MAX_BF_SIZE (((size_t)1) << 29)   /* 64 MB */

/* Total number of blocks to be scanned. */
static guint64 total_blocks;
static guint64 removed_blocks;
static guint64 reachable_blocks;

/*
 * The number of bits in the bloom filter is 4 times the number of all blocks.
 * Let m be the bits in the bf, n be the number of blocks to be added to the bf
 * (the number of live blocks), and k = 3 (closed to optimal for m/n = 4),
 * the probability of false-positive is
 *
 *     p = (1 - e^(-kn/m))^k = 0.15
 *
 * Because m = 4 * total_blocks >= 4 * (live blocks) = 4n, we should have p <= 0.15.
 * Put it another way, we'll clean up at least 85% dead blocks in each gc operation.
 * See http://en.wikipedia.org/wiki/Bloom_filter.
 *
 * Supose we have 8TB space, and the avg block size is 1MB, we'll have 8M blocks, then
 * the size of bf is (8M * 4)/8 = 4MB.
 *
 * If total_blocks is a small number (e.g. < 100), we should try to clean all dead blocks.
 * So we set the minimal size of the bf to 1KB.
 */
static Bloom *
alloc_gc_index ()
{
    size_t size;

    size = (size_t) MAX(total_blocks << 2, 1 << 13);
    size = MIN (size, MAX_BF_SIZE);

    winguf_message ("GC index size is %u Byte.\n", (int)size >> 3);

    return bloom_create (size, 3, 0);
}

typedef struct {
    Bloom *index;
    GHashTable *visited;
#ifndef WINGUFILE_SERVER
    gboolean no_history;
    char remote_end_commit[41];
    char local_end_commit[41];
#endif

#ifdef WINGUFILE_SERVER
    /* > 0: keep a period of history;
     * == 0: only keep data in head commit;
     * < 0: keep all history data.
     */
    gint64 truncate_time;
    gboolean traversed_head;
#endif

    int traversed_commits;
    gint64 traversed_blocks;
    gboolean ignore_errors;
} GCData;

static int
add_blocks_to_index (SeafFSManager *mgr, GCData *data, const char *file_id)
{
    Bloom *index = data->index;
    Seafile *wingufile;
    int i;

    wingufile = winguf_fs_manager_get_wingufile (mgr, file_id);
    if (!wingufile) {
        winguf_warning ("Failed to find file %s.\n", file_id);
        return -1;
    }

    for (i = 0; i < wingufile->n_blocks; ++i) {
        bloom_add (index, wingufile->blk_sha1s[i]);
        ++data->traversed_blocks;
    }

    wingufile_unref (wingufile);

    return 0;
}

static gboolean
fs_callback (SeafFSManager *mgr,
             const char *obj_id,
             int type,
             void *user_data,
             gboolean *stop)
{
    GCData *data = user_data;

    if (data->visited != NULL) {
        if (g_hash_table_lookup (data->visited, obj_id) != NULL) {
            *stop = TRUE;
            return TRUE;
        }

        char *key = g_strdup(obj_id);
        g_hash_table_insert (data->visited, key, key);
    }

    if (type == SEAF_METADATA_TYPE_FILE &&
        add_blocks_to_index (mgr, data, obj_id) < 0)
        return FALSE;

    return TRUE;
}

static gboolean
traverse_commit (SeafCommit *commit, void *vdata, gboolean *stop)
{
    GCData *data = vdata;
    int ret;

#ifndef WINGUFILE_SERVER
    if (data->no_history && 
        (strcmp (commit->commit_id, data->local_end_commit) == 0 ||
         strcmp (commit->commit_id, data->remote_end_commit) == 0)) {
        *stop = TRUE;
        return TRUE;
    }
#endif

#ifdef WINGUFILE_SERVER
    if (data->truncate_time == 0)
    {
        *stop = TRUE;
        /* Stop after traversing the head commit. */
    }
    else if (data->truncate_time > 0 &&
             (gint64)(commit->ctime) < data->truncate_time &&
             data->traversed_head)
    {
        *stop = TRUE;
        return TRUE;
    }

    if (!data->traversed_head)
        data->traversed_head = TRUE;
#endif

    winguf_debug ("Traversed commit %.8s.\n", commit->commit_id);
    ++data->traversed_commits;

    ret = winguf_fs_manager_traverse_tree (winguf->fs_mgr,
                                         commit->root_id,
                                         fs_callback,
                                         data, data->ignore_errors);
    if (ret < 0 && !data->ignore_errors)
        return FALSE;

    return TRUE;
}

static int
populate_gc_index_for_repo (SeafRepo *repo, Bloom *index, gboolean ignore_errors)
{
    GList *branches, *ptr;
    SeafBranch *branch;
    GCData *data;
    int ret = 0;

    winguf_message ("Populating index for repo %.8s.\n", repo->id);

    branches = winguf_branch_manager_get_branch_list (winguf->branch_mgr, repo->id);
    if (branches == NULL) {
        winguf_warning ("[GC] Failed to get branch list of repo %s.\n", repo->id);
        return -1;
    }

    data = g_new0(GCData, 1);
    data->index = index;
    data->visited = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
#ifndef WINGUFILE_SERVER
    data->no_history = TRUE;
    if (data->no_history) {
        char *remote_head = winguf_repo_manager_get_repo_property (repo->manager,
                                                                 repo->id,
                                                                 REPO_REMOTE_HEAD);
        if (remote_head)
            memcpy (data->remote_end_commit, remote_head, 41);
        g_free (remote_head);

        char *local_head = winguf_repo_manager_get_repo_property (repo->manager,
                                                                repo->id,
                                                                REPO_LOCAL_HEAD);
        if (local_head)
            memcpy (data->local_end_commit, local_head, 41);
        g_free (local_head);
    }
#endif

#ifdef WINGUFILE_SERVER
    gint64 truncate_time = winguf_repo_manager_get_repo_truncate_time (repo->manager,
                                                                     repo->id);
    if (truncate_time > 0) {
        winguf_repo_manager_set_repo_valid_since (repo->manager,
                                                repo->id,
                                                truncate_time);
    } else if (truncate_time == 0) {
        /* Only the head commit is valid after GC if no history is kept. */
        SeafCommit *head = winguf_commit_manager_get_commit (winguf->commit_mgr,
                                                           repo->head->commit_id);
        if (head)
            winguf_repo_manager_set_repo_valid_since (repo->manager,
                                                    repo->id,
                                                    head->ctime);
        winguf_commit_unref (head);
    }

    data->truncate_time = truncate_time;
    data->ignore_errors = ignore_errors;
#endif

    for (ptr = branches; ptr != NULL; ptr = ptr->next) {
        branch = ptr->data;
        gboolean res = winguf_commit_manager_traverse_commit_tree (winguf->commit_mgr,
                                                                 branch->commit_id,
                                                                 traverse_commit,
                                                                 data,
                                                                 ignore_errors);
        winguf_branch_unref (branch);
        if (!res && !ignore_errors) {
            ret = -1;
            break;
        }
    }

    winguf_message ("Traversed %d commits, %"G_GINT64_FORMAT" blocks.\n",
                  data->traversed_commits, data->traversed_blocks);
    reachable_blocks += data->traversed_blocks;

    g_list_free (branches);
    g_hash_table_destroy (data->visited);
    g_free (data);

    return ret;
}

#ifndef WINGUFILE_SERVER
static int
populate_gc_index_for_head (const char *head_id, Bloom *index)
{
    SeafCommit *head;
    GCData *data;
    gboolean ret;

    /* We just need to traverse the head for clone tasks. */
    head = winguf_commit_manager_get_commit (winguf->commit_mgr, head_id);
    if (!head) {
        winguf_warning ("Failed to find clone head %s.\n", head_id);
        return -1;
    }

    data = g_new0 (GCData, 1);
    data->index = index;

    ret = winguf_fs_manager_traverse_tree (winguf->fs_mgr,
                                         head->root_id,
                                         fs_callback,
                                         data, FALSE);

    g_free (data);
    winguf_commit_unref (head);
    return ret;
}
#endif

typedef struct {
    Bloom *index;
    int dry_run;
} CheckBlocksData;

static gboolean
check_block_liveness (const char *block_id, void *vdata)
{
    CheckBlocksData *data = vdata;
    Bloom *index = data->index;

    if (!bloom_test (index, block_id)) {
        ++removed_blocks;
        if (!data->dry_run)
            winguf_block_manager_remove_block (winguf->block_mgr, block_id);
    }

    return TRUE;
}

int
gc_core_run (int dry_run, int ignore_errors)
{
    Bloom *index;
    GList *repos = NULL, *clone_heads = NULL, *ptr;
    int ret;

    total_blocks = winguf_block_manager_get_block_number (winguf->block_mgr);
    removed_blocks = 0;

    if (total_blocks == 0) {
        winguf_message ("No blocks. Skip GC.\n");
        return 0;
    }

    winguf_message ("GC started. Total block number is %"G_GUINT64_FORMAT".\n", total_blocks);

    /*
     * Store the index of live blocks in bloom filter to save memory.
     * Since bloom filters only have false-positive, we
     * may skip some garbage blocks, but we won't delete
     * blocks that are still alive.
     */
    index = alloc_gc_index ();
    if (!index) {
        winguf_warning ("GC: Failed to allocate index.\n");
        return -1;
    }

    winguf_message ("Populating index.\n");

    /* If we meet any error when filling in the index, we should bail out.
     */
#ifdef WINGUFILE_SERVER
    repos = winguf_repo_manager_get_repo_list (winguf->repo_mgr, -1, -1, ignore_errors);
    if (!repos) {
        winguf_warning ("Failed to get repo list or no repos.\n");
        return -1;
    }
#else
    repos = winguf_repo_manager_get_repo_list (winguf->repo_mgr, -1, -1);
#endif

    for (ptr = repos; ptr != NULL; ptr = ptr->next) {
        ret = populate_gc_index_for_repo ((SeafRepo *)ptr->data, index,
                                          ignore_errors);
#ifdef WINGUFILE_SERVER
        winguf_repo_unref ((SeafRepo *)ptr->data);
#endif
        if (ret < 0 && !ignore_errors)
            goto out;
    }

#ifndef WINGUFILE_SERVER
    /* If winguf-daemon exits while downloading a new repo, the downloaded new
     * blocks for that repo won't be refered by any repo_id. So after restart
     * those blocks will be GC'ed. To prevent this, we get a list of commit
     * head ids for thoes new repos.
     */
    clone_heads = winguf_transfer_manager_get_clone_heads (winguf->transfer_mgr);
    for (ptr = clone_heads; ptr != NULL; ptr = ptr->next) {
        ret = populate_gc_index_for_head ((char *)ptr->data, index);
        g_free (ptr->data);
        if (ret < 0)
            goto out;
    }
#endif

    if (!dry_run)
        winguf_message ("Scanning and deleting unused blocks.\n");
    else
        winguf_message ("Scanning unused blocks.\n");

    CheckBlocksData data;
    data.index = index;
    data.dry_run = dry_run;

    ret = winguf_block_manager_foreach_block (winguf->block_mgr,
                                            check_block_liveness,
                                            &data);
    if (ret < 0) {
        winguf_warning ("GC: Failed to clean dead blocks.\n");
        goto out;
    }

    if (!dry_run)
        winguf_message ("GC finished. %"G_GUINT64_FORMAT" blocks total, "
                      "about %"G_GUINT64_FORMAT" reachable blocks, "
                      "%"G_GUINT64_FORMAT" blocks are removed.\n",
                      total_blocks, reachable_blocks, removed_blocks);
    else
        winguf_message ("GC finished. %"G_GUINT64_FORMAT" blocks total, "
                      "about %"G_GUINT64_FORMAT" reachable blocks, "
                      "%"G_GUINT64_FORMAT" blocks can be removed.\n",
                      total_blocks, reachable_blocks, removed_blocks);

out:
    bloom_destroy (index);
    g_list_free (repos);
    g_list_free (clone_heads);
    return ret;
}
