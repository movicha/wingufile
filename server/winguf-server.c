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
#include <ccnet/threaded-rpcserver-proc.h>
#include "log.h"
#include "utils.h"

#include "processors/check-tx-slave-v2-proc.h"
#include "processors/check-tx-slave-v3-proc.h"
#include "processors/recvfs-proc.h"
#include "processors/putfs-proc.h"
#include "processors/putblock-v2-proc.h"
#include "processors/recvblock-v2-proc.h"
#include "processors/recvbranch-proc.h"
#include "processors/sync-repo-slave-proc.h"
#include "processors/putcommit-v2-proc.h"
#include "processors/recvcommit-v3-proc.h"
#include "processors/putcs-v2-proc.h"
#include "processors/checkbl-proc.h"

#include "cdc/cdc.h"

SeafileSession *winguf;
SearpcClient *ccnetrpc_client;
SearpcClient *ccnetrpc_client_t;
SearpcClient *async_ccnetrpc_client;
SearpcClient *async_ccnetrpc_client_t;

char *pidfile = NULL;

static const char *short_options = "hvc:d:l:fg:G:P:mCD:";
static struct option long_options[] = {
    { "help", no_argument, NULL, 'h', },
    { "version", no_argument, NULL, 'v', },
    { "config-file", required_argument, NULL, 'c' },
    { "wingufdir", required_argument, NULL, 'd' },
    { "log", required_argument, NULL, 'l' },
    { "debug", required_argument, NULL, 'D' },
    { "foreground", no_argument, NULL, 'f' },
    { "ccnet-debug-level", required_argument, NULL, 'g' },
    { "wingufile-debug-level", required_argument, NULL, 'G' },
    { "master", no_argument, NULL, 'm'},
    { "pidfile", required_argument, NULL, 'P' },
    { "cloud-mode", no_argument, NULL, 'C'},
    { NULL, 0, NULL, 0, },
};

static void usage ()
{
    fprintf (stderr, "usage: winguf-server [-c config_dir] [-d wingufile_dir]\n");
}

static void register_processors (CcnetClient *client)
{
    ccnet_register_service (client, "wingufile-check-tx-slave-v2", "basic",
                            WINGUFILE_TYPE_CHECK_TX_SLAVE_V2_PROC, NULL);
    ccnet_register_service (client, "wingufile-check-tx-slave-v3", "basic",
                            WINGUFILE_TYPE_CHECK_TX_SLAVE_V3_PROC, NULL);
    ccnet_register_service (client, "wingufile-putblock-v2", "basic",
                            WINGUFILE_TYPE_PUTBLOCK_V2_PROC, NULL);
    ccnet_register_service (client, "wingufile-recvblock-v2", "basic",
                            WINGUFILE_TYPE_RECVBLOCK_V2_PROC, NULL);
    ccnet_register_service (client, "wingufile-recvfs", "basic",
                            WINGUFILE_TYPE_RECVFS_PROC, NULL);
    ccnet_register_service (client, "wingufile-putfs", "basic",
                            WINGUFILE_TYPE_PUTFS_PROC, NULL);
    ccnet_register_service (client, "wingufile-recvbranch", "basic",
                            WINGUFILE_TYPE_RECVBRANCH_PROC, NULL);
    ccnet_register_service (client, "wingufile-sync-repo-slave", "basic",
                            WINGUFILE_TYPE_SYNC_REPO_SLAVE_PROC, NULL);
    ccnet_register_service (client, "wingufile-putcommit-v2", "basic",
                            WINGUFILE_TYPE_PUTCOMMIT_V2_PROC, NULL);
    ccnet_register_service (client, "wingufile-recvcommit-v3", "basic",
                            WINGUFILE_TYPE_RECVCOMMIT_V3_PROC, NULL);
    ccnet_register_service (client, "wingufile-putcs-v2", "basic",
                            WINGUFILE_TYPE_PUTCS_V2_PROC, NULL);
    ccnet_register_service (client, "wingufile-checkbl", "basic",
                            WINGUFILE_TYPE_CHECKBL_PROC, NULL);
}

#include <wingurpc.h>
#include "wingurpc-signature.h"
#include "wingurpc-marshal.h"

static void start_rpc_service (CcnetClient *client, int cloud_mode)
{
    wingurpc_server_init (register_marshals);

    wingurpc_create_service ("wingufserv-rpcserver");
    ccnet_register_service (client, "wingufserv-rpcserver", "rpc-inner",
                            CCNET_TYPE_RPCSERVER_PROC, NULL);

    wingurpc_create_service ("wingufserv-threaded-rpcserver");
    ccnet_register_service (client, "wingufserv-threaded-rpcserver", "rpc-inner",
                            CCNET_TYPE_THREADED_RPCSERVER_PROC, NULL);

    /* threaded services */

    /* repo manipulation */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_repo,
                                     "wingufile_get_repo",
                                     wingurpc_signature_object__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_destroy_repo,
                                     "wingufile_destroy_repo",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_repo_list,
                                     "wingufile_get_repo_list",
                                     wingurpc_signature_objlist__int_int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_repo_owner,
                                     "wingufile_get_repo_owner",
                                     wingurpc_signature_string__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_edit_repo,
                                     "wingufile_edit_repo",
                                     wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_is_repo_owner,
                                     "wingufile_is_repo_owner",
                                     wingurpc_signature_int__string_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_owned_repos,
                                     "wingufile_list_owned_repos",
                                     wingurpc_signature_objlist__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_server_repo_size,
                                     "wingufile_server_repo_size",
                                     wingurpc_signature_int64__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_repo_set_access_property,
                                     "wingufile_repo_set_access_property",
                                     wingurpc_signature_int__string_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_repo_query_access_property,
                                     "wingufile_repo_query_access_property",
                                     wingurpc_signature_string__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_revert_on_server,
                                     "wingufile_revert_on_server",
                                     wingurpc_signature_int__string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_diff,
                                     "wingufile_diff",
                                     wingurpc_signature_objlist__string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_post_file,
                                     "wingufile_post_file",
                    wingurpc_signature_int__string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_post_multi_files,
                                     "wingufile_post_multi_files",
                    wingurpc_signature_string__string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_put_file,
                                     "wingufile_put_file",
                    wingurpc_signature_string__string_string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_post_empty_file,
                                     "wingufile_post_empty_file",
                        wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_post_dir,
                                     "wingufile_post_dir",
                        wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_del_file,
                                     "wingufile_del_file",
                        wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_copy_file,
                                     "wingufile_copy_file",
       wingurpc_signature_int__string_string_string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_move_file,
                                     "wingufile_move_file",
       wingurpc_signature_int__string_string_string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_rename_file,
                                     "wingufile_rename_file",
                    wingurpc_signature_int__string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_is_valid_filename,
                                     "wingufile_is_valid_filename",
                                     wingurpc_signature_int__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_create_repo,
                                     "wingufile_create_repo",
                                     wingurpc_signature_string__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_commit,
                                     "wingufile_get_commit",
                                     wingurpc_signature_object__string());
    
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_dir,
                                     "wingufile_list_dir",
                                     wingurpc_signature_objlist__string_int_int());
    
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_file_size,
                                     "wingufile_get_file_size",
                                     wingurpc_signature_int64__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_dir_size,
                                     "wingufile_get_dir_size",
                                     wingurpc_signature_int64__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_dir_by_path,
                                     "wingufile_list_dir_by_path",
                                     wingurpc_signature_objlist__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_dirid_by_path,
                                     "wingufile_get_dirid_by_path",
                                     wingurpc_signature_string__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_file_id_by_path,
                                     "wingufile_get_file_id_by_path",
                                     wingurpc_signature_string__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_dir_id_by_path,
                                     "wingufile_get_dir_id_by_path",
                                     wingurpc_signature_string__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_file_revisions,
                                     "wingufile_list_file_revisions",
                                     wingurpc_signature_objlist__string_string_int_int());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_calc_files_last_modified,
                                     "wingufile_calc_files_last_modified",
                                     wingurpc_signature_objlist__string_string_int());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_revert_file,
                                     "wingufile_revert_file",
                                     wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_revert_dir,
                                     "wingufile_revert_dir",
                                     wingurpc_signature_int__string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_deleted,
                                     "get_deleted",
                                     wingurpc_signature_objlist__string_int());

    /* share repo to user */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_add_share,
                                     "wingufile_add_share",
                                     wingurpc_signature_int__string_string_string_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_share_repos,
                                     "wingufile_list_share_repos",
                                     wingurpc_signature_objlist__string_string_int_int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_org_share_repos,
                                     "wingufile_list_org_share_repos",
                                     wingurpc_signature_objlist__int_string_string_int_int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_remove_share,
                                     "wingufile_remove_share",
                                     wingurpc_signature_int__string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_share_permission,
                                     "set_share_permission",
                                     wingurpc_signature_int__string_string_string_string());

    /* share repo to group */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_group_share_repo,
                                     "wingufile_group_share_repo",
                                     wingurpc_signature_int__string_int_string_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_group_unshare_repo,
                                     "wingufile_group_unshare_repo",
                                     wingurpc_signature_int__string_int_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_shared_groups_by_repo,
                                     "wingufile_get_shared_groups_by_repo",
                                     wingurpc_signature_string__string());
    
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_group_repoids,
                                     "wingufile_get_group_repoids",
                                     wingurpc_signature_string__int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_group_repos_by_owner,
                                     "get_group_repos_by_owner",
                                     wingurpc_signature_objlist__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_group_repo_owner,
                                     "get_group_repo_owner",
                                     wingurpc_signature_string__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_remove_repo_group,
                                     "wingufile_remove_repo_group",
                                     wingurpc_signature_int__int_string());    

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_group_repo_permission,
                                     "set_group_repo_permission",
                                     wingurpc_signature_int__int_string_string());
    
    /* branch and commit */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_branch_gets,
                                     "wingufile_branch_gets",
                                     wingurpc_signature_objlist__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_commit_list,
                                     "wingufile_get_commit_list",
                                     wingurpc_signature_objlist__string_int_int());

    /* token */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_generate_repo_token,
                                     "wingufile_generate_repo_token",
                                     wingurpc_signature_string__string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_delete_repo_token,
                                     "wingufile_delete_repo_token",
                                     wingurpc_signature_int__string_string_string());
    
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_repo_tokens,
                                     "wingufile_list_repo_tokens",
                                     wingurpc_signature_objlist__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_list_repo_tokens_by_email,
                                     "wingufile_list_repo_tokens_by_email",
                                     wingurpc_signature_objlist__string());
    
    /* quota */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_user_quota_usage,
                                     "wingufile_get_user_quota_usage",
                                     wingurpc_signature_int64__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_user_share_usage,
                                     "wingufile_get_user_share_usage",
                                     wingurpc_signature_int64__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_org_quota_usage,
                                     "wingufile_get_org_quota_usage",
                                     wingurpc_signature_int64__int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_org_user_quota_usage,
                                     "wingufile_get_org_user_quota_usage",
                                     wingurpc_signature_int64__int_string());

    /* virtual repo */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_create_virtual_repo,
                                     "create_virtual_repo",
                                     wingurpc_signature_string__string_string_string_string_string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_virtual_repos_by_owner,
                                     "get_virtual_repos_by_owner",
                                     wingurpc_signature_objlist__string());

    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_virtual_repo,
                                     "get_virtual_repo",
                                     wingurpc_signature_object__string_string_string());

    /* -------- rpc services -------- */
    /* token for web access to repo */
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_web_get_access_token,
                                     "wingufile_web_get_access_token",
                                     wingurpc_signature_string__string_string_string_string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_web_query_access_token,
                                     "wingufile_web_query_access_token",
                                     wingurpc_signature_object__string());

    /* chunk server manipulation */
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_add_chunk_server,
                                     "wingufile_add_chunk_server",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_del_chunk_server,
                                     "wingufile_del_chunk_server",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_list_chunk_servers,
                                     "wingufile_list_chunk_servers",
                                     wingurpc_signature_string__void());

    /* set monitor */
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_set_monitor,
                                     "wingufile_set_monitor",
                                     wingurpc_signature_int__string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_get_monitor,
                                     "wingufile_get_monitor",
                                     wingurpc_signature_string__void());

    /* password management */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_passwd,
                                     "wingufile_set_passwd",
                                     wingurpc_signature_int__string_string_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_unset_passwd,
                                     "wingufile_unset_passwd",
                                     wingurpc_signature_int__string_string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_is_passwd_set,
                                     "wingufile_is_passwd_set",
                                     wingurpc_signature_int__string_string());
    wingurpc_server_register_function ("wingufserv-rpcserver",
                                     wingufile_get_decrypt_key,
                                     "wingufile_get_decrypt_key",
                                     wingurpc_signature_object__string_string());

    /* quota management */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_user_quota,
                                     "set_user_quota",
                                     wingurpc_signature_int__string_int64());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_user_quota,
                                     "get_user_quota",
                                     wingurpc_signature_int64__string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_org_quota,
                                     "set_org_quota",
                                     wingurpc_signature_int__int_int64());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_org_quota,
                                     "get_org_quota",
                                     wingurpc_signature_int64__int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_org_user_quota,
                                     "set_org_user_quota",
                                     wingurpc_signature_int__int_string_int64());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_org_user_quota,
                                     "get_org_user_quota",
                                     wingurpc_signature_int64__int_string());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_check_quota,
                                     "check_quota",
                                     wingurpc_signature_int__string());

    /* repo permission */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_check_permission,
                                     "check_permission",
                                     wingurpc_signature_string__string_string());
    
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_file_id_by_commit_and_path,
                                     "wingufile_get_file_id_by_commit_and_path",
                                     wingurpc_signature_string__string_string());

    if (!cloud_mode) {
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_set_inner_pub_repo,
                                         "set_inner_pub_repo",
                                         wingurpc_signature_int__string_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_unset_inner_pub_repo,
                                         "unset_inner_pub_repo",
                                         wingurpc_signature_int__string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_is_inner_pub_repo,
                                         "is_inner_pub_repo",
                                         wingurpc_signature_int__string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_list_inner_pub_repos,
                                         "list_inner_pub_repos",
                                         wingurpc_signature_objlist__void());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_count_inner_pub_repos,
                                         "count_inner_pub_repos",
                                         wingurpc_signature_int64__void());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_list_inner_pub_repos_by_owner,
                                         "list_inner_pub_repos_by_owner",
                                         wingurpc_signature_objlist__string());
    }

    /* Org repo */
    if (cloud_mode) {
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_create_org_repo,
                                         "wingufile_create_org_repo",
            wingurpc_signature_string__string_string_string_string_int());

        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_id_by_repo_id,
                                         "wingufile_get_org_id_by_repo_id",
                                         wingurpc_signature_int__string());

        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_repo_list,
                                         "wingufile_get_org_repo_list",
                                         wingurpc_signature_objlist__int_int_int());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_remove_org_repo_by_org_id,
                                         "wingufile_remove_org_repo_by_org_id",
                                         wingurpc_signature_int__int());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_list_org_repos_by_owner,
                                         "list_org_repos_by_owner",
                                  wingurpc_signature_objlist__int_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_repo_owner,
                                         "get_org_repo_owner",
                                  wingurpc_signature_string__string());

        /* org group repo */
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_add_org_group_repo,
                                         "add_org_group_repo",
            wingurpc_signature_int__string_int_int_string_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_del_org_group_repo,
                                         "del_org_group_repo",
                                         wingurpc_signature_int__string_int_int());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_group_repoids,
                                         "get_org_group_repoids",
                                         wingurpc_signature_string__int_int());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_group_repo_owner,
                                         "get_org_group_repo_owner",
                                wingurpc_signature_string__int_int_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_group_repos_by_owner,
                                         "get_org_group_repos_by_owner",
                                wingurpc_signature_objlist__int_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_get_org_groups_by_repo,
                                         "get_org_groups_by_repo",
                                         wingurpc_signature_string__int_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_set_org_group_repo_permission,
                                         "set_org_group_repo_permission",
                                         wingurpc_signature_int__int_int_string_string());
                                         
        /* org inner pub repo */
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_set_org_inner_pub_repo,
                                         "set_org_inner_pub_repo",
                                         wingurpc_signature_int__int_string_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_unset_org_inner_pub_repo,
                                         "unset_org_inner_pub_repo",
                                         wingurpc_signature_int__int_string());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_list_org_inner_pub_repos,
                                         "list_org_inner_pub_repos",
                                         wingurpc_signature_objlist__int());
        wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                         wingufile_list_org_inner_pub_repos_by_owner,
                                         "list_org_inner_pub_repos_by_owner",
                                         wingurpc_signature_objlist__int_string());
    }

    /* History */
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_set_repo_history_limit,
                                     "set_repo_history_limit",
                                     wingurpc_signature_int__string_int());
    wingurpc_server_register_function ("wingufserv-threaded-rpcserver",
                                     wingufile_get_repo_history_limit,
                                     "get_repo_history_limit",
                                     wingurpc_signature_int__string());
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
    ccnetrpc_client_t = ccnet_create_rpc_client (sync_client,
                                                 NULL,
                                                 "ccnet-threaded-rpcserver");
}

static void
create_async_rpc_clients (CcnetClient *client)
{
    async_ccnetrpc_client = ccnet_create_async_rpc_client (
        client, NULL, "ccnet-rpcserver");
    async_ccnetrpc_client_t = ccnet_create_async_rpc_client (
        client, NULL, "ccnet-threaded-rpcserver");
}

static void
remove_pidfile (const char *pidfile)
{
    if (pidfile) {
        g_unlink (pidfile);
    }
}

static int
write_pidfile (const char *pidfile_path)
{
    if (!pidfile_path)
        return -1;

    pid_t pid = getpid();

    FILE *pidfile = g_fopen(pidfile_path, "w");
    if (!pidfile) {
        winguf_warning ("Failed to fopen() pidfile %s: %s\n",
                      pidfile_path, strerror(errno));
        return -1;
    }

    char buf[32];
    snprintf (buf, sizeof(buf), "%d\n", pid);
    if (fputs(buf, pidfile) < 0) {
        winguf_warning ("Failed to write pidfile %s: %s\n",
                      pidfile_path, strerror(errno));
        return -1;
    }

    fflush (pidfile);
    fclose (pidfile);
    return 0;
}

static void
load_history_config ()
{
    int keep_history_days;
    GError *error = NULL;

    winguf->keep_history_days = -1;

    keep_history_days = g_key_file_get_integer (winguf->config,
                                                "history", "keep_days",
                                                &error);
    if (error == NULL)
        winguf->keep_history_days = keep_history_days;
}

static void
on_winguf_server_exit(void)
{
    if (pidfile)
        remove_pidfile (pidfile);
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

int
main (int argc, char **argv)
{
    int c;
    char *config_dir = DEFAULT_CONFIG_DIR;
    char *wingufile_dir = NULL;
    char *logfile = NULL;
    const char *debug_str = NULL;
    int daemon_mode = 1;
    int is_master = 0;
    CcnetClient *client;
    char *ccnet_debug_level_str = "info";
    char *wingufile_debug_level_str = "debug";
    int cloud_mode = 0;

#ifdef WIN32
    argv = get_argv_utf8 (&argc);
#endif

    while ((c = getopt_long (argc, argv, short_options, 
                             long_options, NULL)) != EOF)
    {
        switch (c) {
        case 'h':
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
        case 'f':
            daemon_mode = 0;
            break;
        case 'l':
            logfile = g_strdup(optarg);
            break;
        case 'D':
            debug_str = optarg;
            break;
        case 'g':
            ccnet_debug_level_str = optarg;
            break;
        case 'G':
            wingufile_debug_level_str = optarg;
            break;
        case 'm':
            is_master = 1;
        case 'P':
            pidfile = optarg;
            break;
        case 'C':
            cloud_mode = 1;
            break;
        default:
            usage ();
            exit (1);
        }
    }

    argc -= optind;
    argv += optind;

#ifndef WIN32
    if (daemon_mode)
        daemon (1, 0);
#endif

    cdc_init ();

    g_type_init ();
#if !GLIB_CHECK_VERSION(2,32,0)
    g_thread_init (NULL);
#endif

    if (!debug_str)
        debug_str = g_getenv("WINGUFILE_DEBUG");
    wingufile_debug_set_flags_string (debug_str);

    if (wingufile_dir == NULL)
        wingufile_dir = g_build_filename (config_dir, "wingufile", NULL);
    if (logfile == NULL)
        logfile = g_build_filename (wingufile_dir, "wingufile.log", NULL);

    if (wingufile_log_init (logfile, ccnet_debug_level_str,
                          wingufile_debug_level_str) < 0) {
        winguf_warning ("Failed to init log.\n");
        exit (1);
    }

    client = ccnet_init (config_dir);
    if (!client)
        exit (1);

    register_processors (client);

    start_rpc_service (client, cloud_mode);

    create_sync_rpc_clients (config_dir);
    create_async_rpc_clients (client);

    winguf = wingufile_session_new (wingufile_dir, client);
    if (!winguf) {
        winguf_warning ("Failed to create wingufile session.\n");
        exit (1);
    }
    winguf->is_master = is_master;
    winguf->ccnetrpc_client = ccnetrpc_client;
    winguf->async_ccnetrpc_client = async_ccnetrpc_client;
    winguf->ccnetrpc_client_t = ccnetrpc_client_t;
    winguf->async_ccnetrpc_client_t = async_ccnetrpc_client_t;
    winguf->client_pool = ccnet_client_pool_new (config_dir);
    winguf->cloud_mode = cloud_mode;

    load_history_config ();

    g_free (wingufile_dir);
    g_free (logfile);

    set_signal_handlers (winguf);

    /* init winguf */
    if (wingufile_session_init (winguf) < 0)
        exit (1);

    if (wingufile_session_start (winguf) < 0)
        exit (1);

    if (pidfile) {
        if (write_pidfile (pidfile) < 0) {
            ccnet_message ("Failed to write pidfile\n");
            return -1;
        }
    }
    atexit (on_winguf_server_exit);

    ccnet_main (client);

    return 0;
}
