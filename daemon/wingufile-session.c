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

#include <ccnet.h>
#include <ccnet/cevent.h>
#include <ccnet/ccnet-object.h>
#include <utils.h>

#include "wingufile-session.h"
#include "wingufile-config.h"
#include "vc-utils.h"
#include "winguf-utils.h"
#include "gc-core.h"
#include "log.h"

#define MAX_THREADS 50

enum {
	REPO_COMMITTED,
    REPO_FETCHED,
    REPO_UPLOADED,
    REPO_WORKTREE_CHECKED,
	LAST_SIGNAL
};

int signals[LAST_SIGNAL];

G_DEFINE_TYPE (WingufileSession, wingufile_session, G_TYPE_OBJECT);


static void
wingufile_session_class_init (WingufileSessionClass *klass)
{

    signals[REPO_COMMITTED] =
        g_signal_new ("repo-committed", WINGUFILE_TYPE_SESSION,
                      G_SIGNAL_RUN_LAST,
                      0,        /* no class singal handler */
                      NULL, NULL, /* no accumulator */
                      g_cclosure_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[REPO_FETCHED] =
        g_signal_new ("repo-fetched", WINGUFILE_TYPE_SESSION,
                      G_SIGNAL_RUN_LAST,
                      0,        /* no class singal handler */
                      NULL, NULL, /* no accumulator */
                      g_cclosure_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1, G_TYPE_POINTER);

    signals[REPO_UPLOADED] =
        g_signal_new ("repo-uploaded", WINGUFILE_TYPE_SESSION,
                      G_SIGNAL_RUN_LAST,
                      0,        /* no class singal handler */
                      NULL, NULL, /* no accumulator */
                      g_cclosure_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1, G_TYPE_POINTER);

}


WingufileSession *
wingufile_session_new(const char *wingufile_dir,
                    const char *worktree_dir,
                    struct _CcnetClient *ccnet_session)
{
    char *abs_wingufile_dir;
    char *abs_worktree_dir;
    char *tmp_file_dir;
    char *db_path;
    sqlite3 *config_db;
    WingufileSession *session = NULL;

#ifndef SEAF_TOOL
    if (!ccnet_session)
        return NULL;
#endif    

    abs_worktree_dir = ccnet_expand_path (worktree_dir);
    abs_wingufile_dir = ccnet_expand_path (wingufile_dir);
    tmp_file_dir = g_build_filename (abs_wingufile_dir, "tmpfiles", NULL);
    db_path = g_build_filename (abs_wingufile_dir, "config.db", NULL);

    if (checkdir_with_mkdir (abs_wingufile_dir) < 0) {
        g_warning ("Config dir %s does not exist and is unable to create\n",
                   abs_wingufile_dir);
        goto onerror;
    }

    if (checkdir_with_mkdir (abs_worktree_dir) < 0) {
        g_warning ("Worktree %s does not exist and is unable to create\n",
                   abs_worktree_dir);
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

    session = g_object_new (WINGUFILE_TYPE_SESSION, NULL);
    session->winguf_dir = abs_wingufile_dir;
    session->tmp_file_dir = tmp_file_dir;
    session->worktree_dir = abs_worktree_dir;
    session->session = ccnet_session;
    session->config_db = config_db;

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

    session->transfer_mgr = winguf_transfer_manager_new (session);
    if (!session->transfer_mgr)
        goto onerror;
    session->clone_mgr = winguf_clone_manager_new (session);
    if (!session->clone_mgr)
        goto onerror;
#ifndef SEAF_TOOL    
    session->sync_mgr = winguf_sync_manager_new (session);
    if (!session->sync_mgr)
        goto onerror;
    session->wt_monitor = winguf_wt_monitor_new (session);
    if (!session->wt_monitor)
        goto onerror;

    session->job_mgr = ccnet_job_manager_new (MAX_THREADS);
    session->ev_mgr = cevent_manager_new ();
    if (!session->ev_mgr)
        goto onerror;
    
    session->mq_mgr = winguf_mq_manager_new (session);
    if (!session->mq_mgr)
        goto onerror;
#endif    

    return session;

onerror:
    free (abs_wingufile_dir);
    free (abs_worktree_dir);
    g_free (tmp_file_dir);
    g_free (db_path);
    g_free (session);
    return NULL;    
}


static void
wingufile_session_init (WingufileSession *session)
{
}

void
wingufile_session_prepare (WingufileSession *session)
{
    /* Start mq manager earlier, so that we can send notifications
     * when start repo manager. */
    winguf_mq_manager_init (session->mq_mgr);
    winguf_commit_manager_init (session->commit_mgr);
    winguf_fs_manager_init (session->fs_mgr);
    winguf_branch_manager_init (session->branch_mgr);
    winguf_repo_manager_init (session->repo_mgr);
    winguf_clone_manager_init (session->clone_mgr);
#ifndef SEAF_TOOL    
    winguf_sync_manager_init (session->sync_mgr);
#endif
    winguf_mq_manager_set_heartbeat_name (session->mq_mgr,
                                        "wingufile.heartbeat");
}

static void
recover_interrupted_merges ()
{
    GList *repos, *ptr;
    SeafRepo *repo;
    SeafRepoMergeInfo info;
    char *err_msg = NULL;
    gboolean unused;

    repos = winguf_repo_manager_get_repo_list (winguf->repo_mgr, -1, -1);
    for (ptr = repos; ptr; ptr = ptr->next) {
        repo = ptr->data;

        if (winguf_repo_manager_get_merge_info (winguf->repo_mgr, repo->id, &info) < 0) {
            g_warning ("Failed to get merge info for repo %s.\n", repo->id);
            continue;
        }

        if (info.in_merge) {
            winguf_message ("Recovering merge for repo %.8s.\n", repo->id);

            /* No one else is holding the lock. */
            pthread_mutex_lock (&repo->lock);
            if (winguf_repo_merge (repo, "master", &err_msg, &unused) < 0) {
                g_free (err_msg);
            }
            pthread_mutex_unlock (&repo->lock);
        }
    }
    g_list_free (repos);
}

static void *
on_start_cleanup_job (void *vdata)
{
    recover_interrupted_merges ();

    gc_core_run (0, 0);

    return vdata;
}

static void
cleanup_job_done (void *vdata)
{
    WingufileSession *session = vdata;

    if (cevent_manager_start (session->ev_mgr) < 0) {
        g_error ("Failed to start event manager.\n");
        return;
    }

    if (winguf_transfer_manager_start (session->transfer_mgr) < 0) {
        g_error ("Failed to start transfer manager.\n");
        return;
    }

    if (winguf_sync_manager_start (session->sync_mgr) < 0) {
        g_error ("Failed to start sync manager.\n");
        return;
    }

    if (winguf_wt_monitor_start (session->wt_monitor) < 0) {
        g_error ("Failed to start worktree monitor.\n");
        return;
    }

    /* Must be after wt monitor, since we may add watch to repo worktree. */
    if (winguf_repo_manager_start (session->repo_mgr) < 0) {
        g_error ("Failed to start repo manager.\n");
        return;
    }

    if (winguf_clone_manager_start (session->clone_mgr) < 0) {
        g_error ("Failed to start clone manager.\n");
        return;
    }

    /* The system is up and running. */
    session->started = TRUE;
}

static void
on_start_cleanup (WingufileSession *session)
{
    ccnet_job_manager_schedule_job (winguf->job_mgr, 
                                    on_start_cleanup_job, 
                                    cleanup_job_done,
                                    session);
}

void
wingufile_session_start (WingufileSession *session)
{
    /* MQ must be started to send heartbeat message to applet. */
    if (winguf_mq_manager_start (session->mq_mgr) < 0) {
        g_error ("Failed to start mq manager.\n");
        return;
    }

    /* Finish cleanup task before anything is run. */
    on_start_cleanup (session);
}

#if 0
void
wingufile_session_add_event (WingufileSession *session, 
                           const char *type,
                           const char *first, ...)
{
    gchar *body;
    va_list args;

    va_start (args, first);
    body = key_value_list_to_json_v (first, args);
    va_end (args);

    CcnetEvent *event = g_object_new (CCNET_TYPE_EVENT, 
                                      "etype", type, 
                                      "body", body, NULL);
    ccnet_client_send_event(session->session, (GObject *)event);
    g_object_unref (event);

    g_free (body);
}
#endif

