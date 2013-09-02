/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"
#define DEBUG_FLAG WINGUFILE_DEBUG_OTHER
#include "log.h"
#include "utils.h"

#include <ccnet.h>
#include <ccnet/ccnet-object.h>

#include "wingufile-session.h"
#include "winguf-db.h"
#include "quota-mgr.h"

static gint64
get_default_quota (GKeyFile *config)
{
    gint quota_gb;

    /* Get default quota configuration in GB. */
    quota_gb = g_key_file_get_integer (config, "quota", "default", NULL);
    if (quota_gb <= 0)
        return INFINITE_QUOTA;

    return quota_gb * ((gint64)1 << 30);
}

SeafQuotaManager *
winguf_quota_manager_new (struct _SeafileSession *session)
{
    SeafQuotaManager *mgr = g_new0 (SeafQuotaManager, 1);
    if (!mgr)
        return NULL;
    mgr->session = session;

    mgr->default_quota = get_default_quota (session->config);
    mgr->calc_share_usage = g_key_file_get_boolean (session->config,
                                                    "quota", "calc_share_usage",
                                                    NULL);

    return mgr;
}

int
winguf_quota_manager_init (SeafQuotaManager *mgr)
{
    SeafDB *db = mgr->session->db;
    const char *sql;

    switch (winguf_db_type(db)) {
    case SEAF_DB_TYPE_PGSQL:
        sql = "CREATE TABLE IF NOT EXISTS UserQuota (\"user\" VARCHAR(255) PRIMARY KEY,"
            "quota BIGINT)";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgQuota (org_id INTEGER PRIMARY KEY,"
            "quota BIGINT)";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgUserQuota (org_id INTEGER,"
            "\"user\" VARCHAR(255), quota BIGINT, PRIMARY KEY (org_id, \"user\"))";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        break;
    case SEAF_DB_TYPE_SQLITE:
        sql = "CREATE TABLE IF NOT EXISTS UserQuota (user VARCHAR(255) PRIMARY KEY,"
            "quota BIGINT)";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgQuota (org_id INTEGER PRIMARY KEY,"
            "quota BIGINT)";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgUserQuota (org_id INTEGER,"
            "user VARCHAR(255), quota BIGINT, PRIMARY KEY (org_id, user))";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        break;
    case SEAF_DB_TYPE_MYSQL:
        sql = "CREATE TABLE IF NOT EXISTS UserQuota (user VARCHAR(255) PRIMARY KEY,"
            "quota BIGINT) ENGINE=INNODB";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgQuota (org_id INTEGER PRIMARY KEY,"
            "quota BIGINT) ENGINE=INNODB";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        sql = "CREATE TABLE IF NOT EXISTS OrgUserQuota (org_id INTEGER,"
            "user VARCHAR(255), quota BIGINT, PRIMARY KEY (org_id, user))"
            "ENGINE=INNODB";
        if (winguf_db_query (db, sql) < 0)
            return -1;

        break;
    }

    return 0;
}

int
winguf_quota_manager_set_user_quota (SeafQuotaManager *mgr,
                                   const char *user,
                                   gint64 quota)
{
    SeafDB *db = mgr->session->db;
    char sql[512];

    if (winguf_db_type(db) == SEAF_DB_TYPE_PGSQL) {
        gboolean err;
        snprintf(sql, sizeof(sql),
                 "SELECT 1 FROM UserQuota WHERE user='%s'", user);
        if (winguf_db_check_for_existence(db, sql, &err))
            snprintf(sql, sizeof(sql),
                     "UPDATE UserQuota SET quota=%"G_GINT64_FORMAT
                     " WHERE user='%s'", quota, user);
        else
            snprintf(sql, sizeof(sql),
                     "INSERT INTO UserQuota VALUES "
                     "('%s', %"G_GINT64_FORMAT")", user, quota);
        if (err)
            return -1;
        return winguf_db_query (db, sql);
    } else {
        snprintf (sql, sizeof(sql),
                  "REPLACE INTO UserQuota VALUES ('%s', %"G_GINT64_FORMAT")",
                  user, quota);
        return winguf_db_query (mgr->session->db, sql);
    }
}

gint64
winguf_quota_manager_get_user_quota (SeafQuotaManager *mgr,
                                   const char *user)
{
    char sql[512];
    gint64 quota;

    snprintf (sql, sizeof(sql),
              "SELECT quota FROM UserQuota WHERE user='%s'",
              user);
    quota = winguf_db_get_int64 (mgr->session->db, sql);
    if (quota <= 0)
        quota = mgr->default_quota;

    return quota;
}

int
winguf_quota_manager_set_org_quota (SeafQuotaManager *mgr,
                                  int org_id,
                                  gint64 quota)
{
    SeafDB *db = mgr->session->db;
    char sql[512];

    if (winguf_db_type(db) == SEAF_DB_TYPE_PGSQL) {
        gboolean err;
        snprintf(sql, sizeof(sql),
                 "SELECT 1 FROM OrgQuota WHERE org_id=%d", org_id);
        if (winguf_db_check_for_existence(db, sql, &err))
            snprintf(sql, sizeof(sql),
                     "UPDATE OrgQuota SET quota=%"G_GINT64_FORMAT
                     " WHERE org_id=%d", quota, org_id);
        else
            snprintf(sql, sizeof(sql),
                     "INSERT INTO OrgQuota VALUES "
                     "(%d, %"G_GINT64_FORMAT")", org_id, quota);
        if (err)
            return -1;
        return winguf_db_query (db, sql);
    } else {
        snprintf (sql, sizeof(sql),
                  "REPLACE INTO OrgQuota VALUES (%d, %"G_GINT64_FORMAT")",
                  org_id, quota);
        return winguf_db_query (mgr->session->db, sql);
    }
}

gint64
winguf_quota_manager_get_org_quota (SeafQuotaManager *mgr,
                                  int org_id)
{
    char sql[512];
    gint64 quota;

    snprintf (sql, sizeof(sql),
              "SELECT quota FROM OrgQuota WHERE org_id='%d'",
              org_id);
    quota = winguf_db_get_int64 (mgr->session->db, sql);
    if (quota <= 0)
        quota = mgr->default_quota;

    return quota;
}

int
winguf_quota_manager_set_org_user_quota (SeafQuotaManager *mgr,
                                       int org_id,
                                       const char *user,
                                       gint64 quota)
{
    SeafDB *db = mgr->session->db;
    char sql[512];

    if (winguf_db_type(db) == SEAF_DB_TYPE_PGSQL) {
        gboolean err;
        snprintf(sql, sizeof(sql),
                 "SELECT 1 FROM OrgUserQuota WHERE org_id=%d AND user='%s'",
                 org_id, user);
        if (winguf_db_check_for_existence(db, sql, &err))
            snprintf(sql, sizeof(sql),
                     "UPDATE OrgUserQuota SET quota=%"G_GINT64_FORMAT
                     " WHERE org_id=%d AND user='%s'", quota, org_id, user);
        else
            snprintf(sql, sizeof(sql),
                     "INSERT INTO OrgQuota VALUES "
                     "(%d, '%s', %"G_GINT64_FORMAT")", org_id, user, quota);
        if (err)
            return -1;
        return winguf_db_query (db, sql);
    } else {
        snprintf (sql, sizeof(sql),
                  "REPLACE INTO OrgUserQuota VALUES ('%d', '%s', %"G_GINT64_FORMAT")",
                  org_id, user, quota);
        return winguf_db_query (mgr->session->db, sql);
    }
}

gint64
winguf_quota_manager_get_org_user_quota (SeafQuotaManager *mgr,
                                       int org_id,
                                       const char *user)
{
    char sql[512];
    gint64 quota;

    snprintf (sql, sizeof(sql),
              "SELECT quota FROM OrgUserQuota WHERE org_id='%d' AND user='%s'",
              org_id, user);
    quota = winguf_db_get_int64 (mgr->session->db, sql);
    /* return org quota if per user quota is not set. */
    if (quota <= 0)
        quota = winguf_quota_manager_get_org_quota (mgr, org_id);

    return quota;
}

int
winguf_quota_manager_check_quota (SeafQuotaManager *mgr,
                                const char *repo_id)
{
    SeafVirtRepo *vinfo;
    const char *r_repo_id = repo_id;
    char *user = NULL;
    int org_id;
    gint64 quota, usage;
    int ret = 0;

    /* If it's a virtual repo, check quota to origin repo. */
    vinfo = winguf_repo_manager_get_virtual_repo_info (winguf->repo_mgr, repo_id);
    if (vinfo)
        r_repo_id = vinfo->origin_repo_id;

    user = winguf_repo_manager_get_repo_owner (winguf->repo_mgr, r_repo_id);
    if (user != NULL) {
        quota = winguf_quota_manager_get_user_quota (mgr, user);
    } else if (winguf->cloud_mode) {
        org_id = winguf_repo_manager_get_repo_org (winguf->repo_mgr, r_repo_id);
        if (org_id < 0) {
            winguf_warning ("Repo %s has no owner.\n", r_repo_id);
            ret = -1;
            goto out;
        }

        quota = winguf_quota_manager_get_org_quota (mgr, org_id);
    } else {
        winguf_warning ("Repo %s has no owner.\n", r_repo_id);
        ret = -1;
        goto out;
    }

    if (quota == INFINITE_QUOTA)
        goto out;

    if (user) {
        if (!mgr->calc_share_usage) {
            usage = winguf_quota_manager_get_user_usage (mgr, user);
        } else {
            gint64 my_usage, share_usage;
            share_usage = winguf_quota_manager_get_user_share_usage (mgr, user);
            if (share_usage < 0) {
                ret = -1;
                goto out;
            }
            my_usage = winguf_quota_manager_get_user_usage (mgr, user);
            if (my_usage < 0) {
                ret = -1;
                goto out;
            }
            usage = my_usage + share_usage;
        }
    } else
        usage = winguf_quota_manager_get_org_usage (mgr, org_id);

    if (usage < 0 || usage >= quota)
        ret = -1;

out:
    winguf_virtual_repo_info_free (vinfo);
    g_free (user);
    return ret;
}

static gboolean
get_total_size (SeafDBRow *row, void *vpsize)
{
    gint64 *psize = vpsize;

    *psize += winguf_db_row_get_column_int64 (row, 0);

    return TRUE;
}

gint64
winguf_quota_manager_get_user_usage (SeafQuotaManager *mgr, const char *user)
{
    char sql[512];
    gint64 total = 0;

    snprintf (sql, sizeof(sql), 
              "SELECT size FROM "
              "RepoOwner o LEFT JOIN VirtualRepo v ON o.repo_id=v.repo_id, "
              "RepoSize WHERE "
              "owner_id='%s' AND o.repo_id=RepoSize.repo_id "
              "AND v.repo_id IS NULL",
              user);
    if (winguf_db_foreach_selected_row (mgr->session->db, sql,
                                      get_total_size, &total) < 0)
        return -1;

    return total;
}

static void
count_group_members (GHashTable *user_hash, GList *members)
{
    GList *p;
    CcnetGroupUser *user;
    const char *user_name;
    int dummy;

    for (p = members; p; p = p->next) {
        user = p->data;
        user_name = ccnet_group_user_get_user_name (user);
        g_hash_table_insert (user_hash, g_strdup(user_name), &dummy);
        /* winguf_debug ("Shared to %s.\n", user_name); */
        g_object_unref (user);
    }

    g_list_free (members);
}

static gint64
repo_share_usage (const char *user, const char *repo_id)
{
    GHashTable *user_hash;
    int dummy;
    GList *personal = NULL, *groups = NULL, *members = NULL, *p;
    SearpcClient *client = NULL;
    gint64 usage = -1;

    /* winguf_debug ("Computing share usage for repo %s.\n", repo_id); */

    /* If a repo is shared to both a user and a group, and that user is also
     * a member of the group, we don't want to count that user twice.
     * This also applies to two groups with overlapped members.
     * So we have to use a hash table to filter out duplicated users.
     */
    user_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

    /* First count personal share */
    personal = winguf_share_manager_list_shared_to (winguf->share_mgr, user, repo_id);
    for (p = personal; p; p = p->next) {
        char *email = p->data;
        g_hash_table_insert (user_hash, g_strdup(email), &dummy);
        /* winguf_debug ("Shared to %s.\n", email); */
    }

    /* Then groups... */
    client = ccnet_create_pooled_rpc_client (winguf->client_pool,
                                             NULL,
                                             "ccnet-threaded-rpcserver");
    if (!client) {
        winguf_warning ("Failed to alloc rpc client.\n");
        goto out;
    }

    groups = winguf_repo_manager_get_groups_by_repo (winguf->repo_mgr,
                                                   repo_id, NULL);
    for (p = groups; p; p = p->next) {
        members = ccnet_get_group_members (client, (int)p->data);
        if (!members) {
            winguf_warning ("Cannot get member list for groupd %d.\n", (int)p->data);
            goto out;
        }

        count_group_members (user_hash, members);
    }

    /* Remove myself if i'm in a group. */
    g_hash_table_remove (user_hash, user);

    guint n_shared_to = g_hash_table_size(user_hash);
    /* winguf_debug ("n_shared_to = %u.\n", n_shared_to); */
    if (n_shared_to == 0) {
        usage = 0;
        goto out;
    }

    gint64 size = winguf_repo_manager_get_repo_size (winguf->repo_mgr, repo_id);
    if (size < 0) {
        winguf_warning ("Cannot get size of repo %s.\n", repo_id);
        goto out;
    }

    /* share_usage = repo_size * n_shared_to */
    usage = size * n_shared_to;

out:
    g_hash_table_destroy (user_hash);
    string_list_free (personal);
    g_list_free (groups);
    ccnet_rpc_client_free (client);
    return usage;
}

gint64
winguf_quota_manager_get_user_share_usage (SeafQuotaManager *mgr,
                                         const char *user)
{
    GList *repos, *p;
    char *repo_id;
    gint64 total = 0, per_repo;

    repos = winguf_repo_manager_get_repo_ids_by_owner (winguf->repo_mgr, user);

    for (p = repos; p != NULL; p = p->next) {
        repo_id = p->data;
        per_repo = repo_share_usage (user, repo_id);
        if (per_repo < 0) {
            winguf_warning ("Failed to get repo %s share usage.\n", repo_id);
            string_list_free (repos);
            return -1;
        }

        total += per_repo;
    }

    string_list_free (repos);
    return total;
}

gint64
winguf_quota_manager_get_org_usage (SeafQuotaManager *mgr, int org_id)
{
    char sql[256];
    gint64 ret = 0;

    snprintf (sql, sizeof(sql), 
              "SELECT size FROM OrgRepo, RepoSize WHERE "
              "org_id=%d AND OrgRepo.repo_id=RepoSize.repo_id",
              org_id);
    if (winguf_db_foreach_selected_row (mgr->session->db, sql,
                                      get_total_size, &ret) < 0)
        return -1;

    return ret;
}

gint64
winguf_quota_manager_get_org_user_usage (SeafQuotaManager *mgr,
                                       int org_id,
                                       const char *user)
{
    char sql[256];
    gint64 ret = 0;

    snprintf (sql, sizeof(sql), 
              "SELECT size FROM OrgRepo, RepoSize WHERE "
              "org_id=%d AND user = '%s' AND OrgRepo.repo_id=RepoSize.repo_id",
              org_id, user);
    if (winguf_db_foreach_selected_row (mgr->session->db, sql,
                                      get_total_size, &ret) < 0)
        return -1;

    return ret;
}
