/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>

#include <glib.h>
#include <glib-object.h>

#include <ccnet.h>
#include <wingurpc-server.h>
#include <wingurpc-client.h>

#include "wingufile-session.h"
#include "wingufile-rpc.h"
#include <ccnet/rpcserver-proc.h>
#include "log.h"
#include "utils.h"
#include "vc-utils.h"
#include "wingufile-config.h"

#include "processors/notifysync-slave-proc.h"
#include "processors/sync-repo-slave-proc.h"
#include "processors/check-tx-slave-proc.h"
#include "processors/putcommit-proc.h"
#include "processors/putfs-proc.h"

#include "cdc/cdc.h"

#ifndef WINGUFILE_CLIENT_VERSION
#define WINGUFILE_CLIENT_VERSION PACKAGE_VERSION
#endif


SeafileSession *winguf;
SearpcClient *ccnetrpc_client;
SearpcClient *appletrpc_client;
CcnetClient *bind_client;

static const char *short_options = "hvc:d:w:l:D:bg:G:";
static struct option long_options[] = {
    { "help", no_argument, NULL, 'h', },
    { "version", no_argument, NULL, 'v', },
    { "config-file", required_argument, NULL, 'c' },
    { "wingufdir", required_argument, NULL, 'd' },
    { "daemon", no_argument, NULL, 'b' },
    { "debug", required_argument, NULL, 'D' },
    { "worktree", required_argument, NULL, 'w' },
    { "log", required_argument, NULL, 'l' },
    { "ccnet-debug-level", required_argument, NULL, 'g' },
    { "wingufile-debug-level", required_argument, NULL, 'G' },
    { NULL, 0, NULL, 0, },
};

static void usage ()
{
    fprintf (stderr, "usage: winguf-daemon [-c config_dir] [-d wingufile_dir] [-w worktree_dir] [--daemon]\n");
}

#include <wingurpc.h>
#include "wingurpc-signature.h"
#include "wingurpc-marshal.h"

static void
start_rpc_service (CcnetClient *client)
{
    wingurpc_server_init (register_marshals);

    wingurpc_create_service ("wingufile-rpcserver");
    ccnet_register_service (client, "wingufile-rpcserver", "rpc-inner",
                            CCNET_TYPE_RPCSERVER_PROC, NULL);

    /* wingurpc_create_service ("wingufile-threaded-rpcserver"); */
    /* ccnet_register_service (client, "wingufile-threaded-rpcserver", "rpc-inner", */
    /*                         CCNET_TYPE_THREADED_RPCSERVER_PROC, */
    /*                         wingufile_register_service_cb); */

    /* wingufile-rpcserver */
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_session_info,
                                     "wingufile_get_session_info",
                                     wingurpc_signature_object__void());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_config,
                                     "wingufile_get_config",
                                     wingurpc_signature_string__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_set_config,
                                     "wingufile_set_config",
                                     wingurpc_signature_int__string_string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_config_int,
                                     "wingufile_get_config_int",
                                     wingurpc_signature_int__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_set_config_int,
                                     "wingufile_set_config_int",
                                     wingurpc_signature_int__string_int());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_set_upload_rate_limit,
                                     "wingufile_set_upload_rate_limit",
                                     wingurpc_signature_int__int());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_set_download_rate_limit,
                                     "wingufile_set_download_rate_limit",
                                     wingurpc_signature_int__int());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_destroy_repo,
                                     "wingufile_destroy_repo",
                                     wingurpc_signature_int__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_set_repo_property,
                                     "wingufile_set_repo_property",
                                     wingurpc_signature_int__string_string_string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_property,
                                     "wingufile_get_repo_property",
                                     wingurpc_signature_string__string_string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_relay_address,
                                     "wingufile_get_repo_relay_address",
                                     wingurpc_signature_string__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_relay_port,
                                     "wingufile_get_repo_relay_port",
                                     wingurpc_signature_string__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_update_repo_relay_info,
                                     "wingufile_update_repo_relay_info",
                                     wingurpc_signature_int__string_string_string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_disable_auto_sync,
                                     "wingufile_disable_auto_sync",
                                     wingurpc_signature_int__void());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_enable_auto_sync,
                                     "wingufile_enable_auto_sync",
                                     wingurpc_signature_int__void());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_is_auto_sync_enabled,
                                     "wingufile_is_auto_sync_enabled",
                                     wingurpc_signature_int__void());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_branch_gets,
                                     "wingufile_branch_gets",
                                     wingurpc_signature_objlist__string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_gen_default_worktree,
                                     "gen_default_worktree",
                                     wingurpc_signature_string__string_string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_clone,
                                     "wingufile_clone",
        wingurpc_signature_string__string_string_string_string_string_string_string_string_string_string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_download,
                                     "wingufile_download",
        wingurpc_signature_string__string_string_string_string_string_string_string_string_string_string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_cancel_clone_task,
                                     "wingufile_cancel_clone_task",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_remove_clone_task,
                                     "wingufile_remove_clone_task",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_clone_tasks,
                                     "wingufile_get_clone_tasks",
                                     wingurpc_signature_objlist__void());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_sync,
                                     "wingufile_sync",
                                     wingurpc_signature_int__string_string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_list,
                                     "wingufile_get_repo_list",
                                     wingurpc_signature_objlist__int_int());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo,
                                     "wingufile_get_repo",
                                     wingurpc_signature_object__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_sync_task_list,
                                     "wingufile_get_sync_task_list",
                                     wingurpc_signature_objlist__void());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_sync_task,
                                     "wingufile_get_repo_sync_task",
                                     wingurpc_signature_object__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_repo_sync_info,
                                     "wingufile_get_repo_sync_info",
                                     wingurpc_signature_object__string());

    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_commit,
                                     "wingufile_get_commit",
                                     wingurpc_signature_object__string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_commit_list,
                                     "wingufile_get_commit_list",
                                     wingurpc_signature_objlist__string_int_int());


    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_find_transfer_task,
                                     "wingufile_find_transfer_task",
                                     wingurpc_signature_object__string());
    wingurpc_server_register_function ("wingufile-rpcserver",
                                     wingufile_get_checkout_task,
                                     "wingufile_get_checkout_task",
                                     wingurpc_signature_object__string());

}

static void
set_signal_handlers (SeafileSession *session)
{
#ifndef WIN32
    signal (SIGPIPE, SIG_IGN);
#endif
}

static void
create_sync_rpc_clients (const char *config_dir)
{
    CcnetClient *sync_client;

    /* sync client and rpc client */
    sync_client = ccnet_client_new ();
    if ( (ccnet_client_load_confdir(sync_client, config_dir)) < 0 ) {
        winguf_warning ("Read config dir error\n");
        exit(1);
    }

    if (ccnet_client_connect_daemon (sync_client, CCNET_CLIENT_SYNC) < 0)
    {
        winguf_warning ("Connect to server fail: %s\n", strerror(errno));
        exit(1);
    }

    ccnetrpc_client = ccnet_create_rpc_client (sync_client, NULL, "ccnet-rpcserver");
}

#ifdef WIN32
/* Get the commandline arguments in unicode, then convert them to utf8  */
static char **
get_argv_utf8 (int *argc)
{
    int i = 0;
    char **argv = NULL;
    const wchar_t *cmdline = NULL;
    wchar_t **argv_w = NULL;

    cmdline = GetCommandLineW();
    argv_w = CommandLineToArgvW (cmdline, argc); 
    if (!argv_w) {
        printf("failed to CommandLineToArgvW(), GLE=%lu\n", GetLastError());
        return NULL;
    }

    argv = (char **)malloc (sizeof(char*) * (*argc));
    for (i = 0; i < *argc; i++) {
        argv[i] = wchar_to_utf8 (argv_w[i]);
    }

    return argv;
}
#endif

/*
 * Bind to an unused service to make sure only one instance of winguf-daemon
 * is running.
 */
static gboolean
bind_ccnet_service (const char *config_dir)
{
    gboolean ret = TRUE;

    bind_client = ccnet_client_new ();
    if ( (ccnet_client_load_confdir(bind_client, config_dir)) < 0 ) {
        winguf_warning ("Read config dir error\n");
        exit(1);
    }

    if (ccnet_client_connect_daemon (bind_client, CCNET_CLIENT_SYNC) < 0)
    {
        winguf_warning ("Connect to server fail: %s\n", strerror(errno));
        exit(1);
    }

    if (!ccnet_register_service_sync (bind_client,
                                      "wingufile-dummy-service",
                                      "rpc-inner"))
        ret = FALSE;

    return ret;
}

int
main (int argc, char **argv)
{
    int c;
    char *config_dir = DEFAULT_CONFIG_DIR;
    char *wingufile_dir = NULL;
    char *worktree_dir = NULL;
    char *logfile = NULL;
    const char *debug_str = NULL;
    int daemon_mode = 0;
    CcnetClient *client;
    char *ccnet_debug_level_str = "info";
    char *wingufile_debug_level_str = "debug";

#ifdef WIN32
    argv = get_argv_utf8 (&argc);
#endif

    while ((c = getopt_long (argc, argv, short_options, 
                             long_options, NULL)) != EOF)
    {
        switch (c) {
        case 'h':
            usage();
            exit (1);
            break;
        case 'v':
            exit (1);
            break;
        case 'c':
            config_dir = optarg;
            break;
        case 'd':
            wingufile_dir = g_strdup(optarg);
            break;
        case 'b':
            daemon_mode = 1;
            break;
        case 'D':
            debug_str = optarg;
            break;
        case 'w':
            worktree_dir = g_strdup(optarg);
            break;
        case 'l':
            logfile = g_strdup(optarg);
            break;
        case 'g':
            ccnet_debug_level_str = optarg;
            break;
        case 'G':
            wingufile_debug_level_str = optarg;
            break;
        default:
            usage ();
            exit (1);
        }
    }

    argc -= optind;
    argv += optind;

#ifndef WIN32

#ifndef __APPLE__
    if (daemon_mode)
        daemon (1, 0);
#endif

#endif

    cdc_init ();

    g_type_init ();
#if !GLIB_CHECK_VERSION(2,32,0)
    g_thread_init (NULL);
#endif
    if (!debug_str)
        debug_str = g_getenv("WINGUFILE_DEBUG");
    wingufile_debug_set_flags_string (debug_str);

    if (logfile == NULL)
        logfile = g_build_filename (config_dir, "logs", "wingufile.log", NULL);
    if (wingufile_log_init (logfile, ccnet_debug_level_str,
                          wingufile_debug_level_str) < 0) {
        winguf_warning ("Failed to init log.\n");
        exit (1);
    }

    if (!bind_ccnet_service (config_dir)) {
        winguf_warning ("Failed to bind ccnet service\n");
        exit (1);
    }

    /* init ccnet */
    client = ccnet_init (config_dir);
    if (!client)
        exit (1);

    start_rpc_service (client);

    create_sync_rpc_clients (config_dir);
    appletrpc_client = ccnet_create_async_rpc_client (client, NULL, 
                                                      "applet-rpcserver");

    /* init wingufile */
    if (wingufile_dir == NULL)
        wingufile_dir = g_build_filename (config_dir, "wingufile-data", NULL);
    if (worktree_dir == NULL)
        worktree_dir = g_build_filename (g_get_home_dir(), "wingufile", NULL);

    winguf = wingufile_session_new (wingufile_dir, worktree_dir, client);
    if (!winguf) {
        winguf_warning ("Failed to create wingufile session.\n");
        exit (1);
    }
    winguf->ccnetrpc_client = ccnetrpc_client;
    winguf->appletrpc_client = appletrpc_client;

    winguf_message ("starting wingufile client "WINGUFILE_CLIENT_VERSION"\n");
#if defined(WINGUFILE_SOURCE_COMMIT_ID)
    winguf_message ("wingufile source code version "WINGUFILE_SOURCE_COMMIT_ID"\n");
#endif

    g_free (wingufile_dir);
    g_free (worktree_dir);
    g_free (logfile);

    set_signal_handlers (winguf);

    wingufile_session_prepare (winguf);
    wingufile_session_start (winguf);

    wingufile_session_config_set_string (winguf, "wktree", winguf->worktree_dir);
    ccnet_main (client);

    return 0;
}
