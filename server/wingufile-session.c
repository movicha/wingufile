/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include <stdint.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include <ccnet/cevent.h>
#include <utils.h>

#include "wingufile-session.h"
#include "wingufile-config.h"

#include "monitor-rpc-wrappers.h"

#include "winguf-db.h"
#include "winguf-utils.h"

#define CONNECT_INTERVAL_MSEC 10 * 1000

#define DEFAULT_THREAD_POOL_SIZE 50

static void
load_monitor_id (WingufileSession *session);

static int
load_thread_pool_config (WingufileSession *session);

WingufileSession *
wingufile_session_new(const char *wingufile_dir,
                    CcnetClient *ccnet_session)
{
    char *abs_wingufile_dir;
    char *tmp_file_dir;
    char *config_file_path;
    char *db_path;
    sqlite3 *config_db;
    GKeyFile *config;
    WingufileSession *session = NULL;

    if (!ccnet_session)
        return NULL;

    abs_wingufile_dir = ccnet_expand_path (wingufile_dir);
    tmp_file_dir = g_build_filename (abs_wingufile_dir, "tmpfiles", NULL);
    config_file_path = g_build_filename (abs_wingufile_dir, "wingufile.conf", NULL);
    db_path = g_build_filename (abs_wingufile_dir, "config.db", NULL);

    if (checkdir_with_mkdir (abs_wingufile_dir) < 0) {
        g_warning ("Config dir %s does not exist and is unable to create\n",
                   abs_wingufile_dir);
        goto onerror;
    }

    if (checkdir_with_mkdir (tmp_file_dir) < 0) {
        g_warning ("Temp file dir %s does not exist and is unable to create\n",
                   tmp_file_dir);
        goto onerror;
    }

    config_db = wingufile_session_config_open_db (db_path);
    if (!config_db) {
        g_warning ("Failed to open config db.\n");
        goto onerror;
    }

    GError *error = NULL;
    config = g_key_file_new ();
    if (!g_key_file_load_from_file (config, config_file_path, 
                                    G_KEY_FILE_NONE, &error)) {
        g_warning ("Failed to load config file.\n");
        g_key_file_free (config);
        goto onerror;
    }

    session = g_new0(WingufileSession, 1);
    session->winguf_dir = abs_wingufile_dir;
    session->tmp_file_dir = tmp_file_dir;
    session->session = ccnet_session;
    session->config_db = config_db;
    session->config = config;

    load_monitor_id (session);

    if (load_database_config (session) < 0) {
        g_warning ("Failed to load database config.\n");
        goto onerror;
    }

    if (load_thread_pool_config (session) < 0) {
        g_warning ("Failed to load thread pool config.\n");
        goto onerror;
    }

    session->fs_mgr = winguf_fs_manager_new (session, abs_wingufile_dir);
    if (!session->fs_mgr)
        goto onerror;
    session->block_mgr = winguf_block_manager_new (session, abs_wingufile_dir);
    if (!session->block_mgr)
        goto onerror;
    session->commit_mgr = winguf_commit_manager_new (session);
    if (!session->commit_mgr)
        goto onerror;
    session->repo_mgr = winguf_repo_manager_new (session);
    if (!session->repo_mgr)
        goto onerror;
    session->branch_mgr = winguf_branch_manager_new (session);
    if (!session->branch_mgr)
        goto onerror;

    session->cs_mgr = winguf_cs_manager_new (session);
    if (!session->cs_mgr)
        goto onerror;

    session->share_mgr = winguf_share_manager_new (session);
    if (!session->share_mgr)
        goto onerror;
    
    session->web_at_mgr = winguf_web_at_manager_new (session);
    if (!session->web_at_mgr)
        goto onerror;

    session->token_mgr = winguf_token_manager_new (session);
    if (!session->token_mgr)
        goto onerror;

    session->passwd_mgr = winguf_passwd_manager_new (session);
    if (!session->passwd_mgr)
        goto onerror;

    session->quota_mgr = winguf_quota_manager_new (session);
    if (!session->quota_mgr)
        goto onerror;

    session->listen_mgr = winguf_listen_manager_new (session);
    if (!session->listen_mgr)
        goto onerror;

    session->job_mgr = ccnet_job_manager_new (session->sync_thread_pool_size);
    ccnet_session->job_mgr = ccnet_job_manager_new (session->rpc_thread_pool_size);

    session->size_sched = size_scheduler_new (session);

    session->ev_mgr = cevent_manager_new ();
    if (!session->ev_mgr)
        goto onerror;

    session->mq_mgr = winguf_mq_manager_new (session);
    if (!session->mq_mgr)
        goto onerror;

    return session;

onerror:
    free (abs_wingufile_dir);
    g_free (tmp_file_dir);
    g_free (config_file_path);
    g_free (db_path);
    g_free (session);
    return NULL;    
}

int
wingufile_session_init (WingufileSession *session)
{
    if (winguf_commit_manager_init (session->commit_mgr) < 0)
        return -1;

    if (winguf_fs_manager_init (session->fs_mgr) < 0)
        return -1;

    if (winguf_branch_manager_init (session->branch_mgr) < 0)
        return -1;

    if (winguf_repo_manager_init (session->repo_mgr) < 0)
        return -1;

    if (winguf_quota_manager_init (session->quota_mgr) < 0)
        return -1;

    winguf_mq_manager_init (session->mq_mgr);
    winguf_mq_manager_set_heartbeat_name (session->mq_mgr,
                                        "winguf_server.heartbeat");

    return 0;
}

int
wingufile_session_start (WingufileSession *session)
{
    if (cevent_manager_start (session->ev_mgr) < 0) {
        g_error ("Failed to start event manager.\n");
        return -1;
    }

    if (winguf_cs_manager_start (session->cs_mgr) < 0) {
        g_error ("Failed to start chunk server manager.\n");
        return -1;
    }

    if (winguf_share_manager_start (session->share_mgr) < 0) {
        g_error ("Failed to start share manager.\n");
        return -1;
    }

    if (winguf_web_at_manager_start (session->web_at_mgr) < 0) {
        g_error ("Failed to start web access check manager.\n");
        return -1;
    }

    if (winguf_passwd_manager_start (session->passwd_mgr) < 0) {
        g_error ("Failed to start password manager.\n");
        return -1;
    }

    if (winguf_mq_manager_start (session->mq_mgr) < 0) {
        g_error ("Failed to start mq manager.\n");
        return -1;
    }

    if (winguf_listen_manager_start (session->listen_mgr) < 0) {
        g_error ("Failed to start listen manager.\n");
        return -1;
    }

    if (size_scheduler_start (session->size_sched) < 0) {
        g_error ("Failed to start size scheduler.\n");
        return -1;
    }

    return 0;
}

int
wingufile_session_set_monitor (WingufileSession *session, const char *peer_id)
{
    if (wingufile_session_config_set_string (session, 
                                           KEY_MONITOR_ID,
                                           peer_id) < 0) {
        g_warning ("Failed to set monitor id.\n");
        return -1;
    }

    session->monitor_id = g_strdup(peer_id);
    return 0;
}

static void
load_monitor_id (WingufileSession *session)
{
    char *monitor_id;

    monitor_id = wingufile_session_config_get_string (session, 
                                                    KEY_MONITOR_ID);

    if (monitor_id) {
        session->monitor_id = monitor_id;
    } else {
        /* Set monitor to myself if not set by user. */
        session->monitor_id = g_strdup(session->session->base.id);
    }
}

static int
load_thread_pool_config (WingufileSession *session)
{
    int rpc_tp_size, sync_tp_size;

    rpc_tp_size = g_key_file_get_integer (session->config,
                                          "thread pool size", "rpc",
                                          NULL);
    sync_tp_size = g_key_file_get_integer (session->config,
                                           "thread pool size", "sync",
                                           NULL);

    if (rpc_tp_size > 0)
        session->rpc_thread_pool_size = rpc_tp_size;
    else
        session->rpc_thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    if (sync_tp_size > 0)
        session->sync_thread_pool_size = sync_tp_size;
    else
        session->sync_thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    return 0;
}
