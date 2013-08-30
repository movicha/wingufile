/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <config.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include <searpc-client.h>
#include "wingufile-object.h"

char *
wingufile_get_config (SearpcClient *client, const char *key)
{
    if (!key)
        return NULL;

    return searpc_client_call__string (
        client, "wingufile_get_config", NULL, 
        1, "string", key);
}

int
wingufile_get_config_async (SearpcClient *client, const char *key,
                          AsyncCallback callback, void *user_data)
{
    if (!key)
        return -1;

    return searpc_client_async_call__string (
        client, "wingufile_get_config", callback, user_data,
        1, "string", key);
}

int
wingufile_set_config (SearpcClient *client, const char *key, const char *value)
{
    if (!key || !value)
        return -1;

    return searpc_client_call__int (
        client, "wingufile_set_config", NULL,
        2, "string", key, "string", value);
}


int wingufile_set_config_async (SearpcClient *client,
                              const char *key, const char *value,
                              AsyncCallback callback, void *user_data)
{
    if (!key || !value)
        return -1;

    return searpc_client_async_call__int (
        client, "wingufile_set_config", callback, user_data,
        2, "string", key, "string", value);   
}

char *
wingufile_create_repo (SearpcClient *client,
                     const gchar *name, 
                     const gchar *description,
                     const gchar *worktree,
                     const gchar *passwd,
                     const gchar *relay_id,
                     int keep_local_history,
                     GError **error)
{
    g_return_val_if_fail (client && name && description && worktree, NULL);

    return searpc_client_call__string (
        client, "wingufile_create_repo", error,
        6, "string", name, "string", description,
        "string", worktree, "string", passwd,
        "string", relay_id, "int", keep_local_history);
}

int
wingufile_create_repo_async (SearpcClient *client,
                           const gchar *name, 
                           const gchar *description,
                           const gchar *worktree,
                           const gchar *passwd,
                           const gchar *relay_id,
                           int keep_local_history,
                           AsyncCallback callback, void *user_data)
{
    g_return_val_if_fail (client && name && description && worktree, -1);

    return searpc_client_async_call__string (
        client, "wingufile_create_repo", callback, user_data,
        6, "string", name, "string", description,
        "string", worktree, "string", passwd,
        "string", relay_id, "int", keep_local_history);
}

int
wingufile_destroy_repo (SearpcClient *client,
                      const char *repo_id, GError **error)
{
    g_return_val_if_fail (client && repo_id, -1);

    return searpc_client_call__int (
        client, "wingufile_destroy_repo", error,
        1, "string", repo_id);
}

int
wingufile_set_repo_token (SearpcClient *client,
                        const char *repo_id,
                        const char *token,
                        GError **error)
{
    g_return_val_if_fail (client && repo_id && token, -1);

    return searpc_client_call__int (
        client, "wingufile_set_repo_token", error,
        2, "string", repo_id, "string", token);
}

char *
wingufile_get_repo_token (SearpcClient *client,
                        const char *repo_id,
                        GError **error)
{
    g_return_val_if_fail (client && repo_id, NULL);

    return searpc_client_call__string (
        client, "wingufile_get_repo_token", error,
        1, "string", repo_id);
}

GList *
wingufile_get_repo_list (SearpcClient *client,
                       int offset,
                       int limit, GError **error)
{
    return searpc_client_call__objlist (
        client, "wingufile_get_repo_list", WINGUFILE_TYPE_REPO, error,
        2, "int", offset, "int", limit);
}

GObject *
wingufile_get_repo (SearpcClient *client,
                  const char *repo_id,
                  GError **error)
{
    g_return_val_if_fail (client && repo_id, NULL);

    return searpc_client_call__object (
        client, "wingufile_get_repo", WINGUFILE_TYPE_REPO, error,
        1, "string", repo_id);
}

int
wingufile_set_repo_property (SearpcClient *client,
                           const char *repo_id,
                           const char *key,
                           const char *value,
                           GError **error)
{
    g_return_val_if_fail (client && repo_id && key, -1);

    return searpc_client_call__int (
        client, "wingufile_set_repo_property", error,
        3, "string", repo_id, "string", key, "string", value);
}

char *
wingufile_get_repo_property (SearpcClient *client,
                           const char *repo_id,
                           const char *key,
                           GError **error)
{
    g_return_val_if_fail (client && repo_id, NULL);

    return searpc_client_call__string (
        client, "wingufile_get_repo_property", error,
        2, "string", repo_id, "string", key);
}


int
wingufile_calc_dir_size (SearpcClient *client, const char *path, GError **error)
{
    return searpc_client_call__int (client, "wingufile_calc_dir_size", error,
                                    1, "string", path);
}


int
wingufile_add_chunk_server (SearpcClient *client,
                          const char *server_id, GError **error)
{
    if (!server_id)
        return -1;

    return searpc_client_call__int (
        client, "wingufile_add_chunk_server", error, 
        1, "string", server_id);
}

int
wingufile_del_chunk_server (SearpcClient *client,
                          const char *server_id, GError **error)
{
    if (!server_id)
        return -1;

    return searpc_client_call__int (
        client, "wingufile_del_chunk_server", error, 
        1, "string", server_id);
}

char *
wingufile_list_chunk_servers (SearpcClient *client, GError **error)
{
    return searpc_client_call__string (
        client, "wingufile_list_chunk_servers", error,
        0);
}

int
wingufile_set_monitor (SearpcClient *client, const char *peer_id,
                     GError **error)
{
    if (!peer_id)
        return -1;

    return searpc_client_call__int (
        client, "wingufile_set_monitor", error, 
        1, "string", peer_id);
}

char *
wingufile_get_monitor (SearpcClient *client, GError **error)
{
    return searpc_client_call__string (
        client, "wingufile_get_monitor", error,
        0);
}


char *
wingufile_repo_query_access_property (SearpcClient *client,
                                    const char *repo_id,
                                    GError **error)
{
    return searpc_client_call__string (
        client, "wingufile_repo_query_access_property", error,
        1, "string", repo_id);
}

GObject *
wingufile_web_query_access_token (SearpcClient *client,
                                const char *token,
                                GError **error)
{
    return searpc_client_call__object (
        client, "wingufile_web_query_access_token", WINGUFILE_TYPE_WEB_ACCESS, error,
        1, "string", token);
}

GObject *
wingufile_get_decrypt_key (SearpcClient *client,
                         const char *repo_id,
                         const char *user,
                         GError **error)
{
    return searpc_client_call__object (
        client, "wingufile_get_decrypt_key", WINGUFILE_TYPE_CRYPT_KEY, error,
        2, "string", repo_id, "string", user);
}

char *
wingufile_put_file (SearpcClient *client,
                  const char *repo_id,
                  const char *file_path,
                  const char *parent_dir,
                  const char *file_name,
                  const char *user,
                  const char *head_id,
                  GError **error)
{
    return searpc_client_call__string (client, "wingufile_put_file", error,
                                    6, "string", repo_id,
                                    "string", file_path,
                                    "string", parent_dir,
                                    "string", file_name,
                                    "string", user,
                                    "string", head_id);
}

int
wingufile_post_file (SearpcClient *client,
                   const char *repo_id,
                   const char *file_path,
                   const char *parent_dir,
                   const char *file_name,
                   const char *user,
                   GError **error)
{
    return searpc_client_call__int (client, "wingufile_post_file", error,
                                    5, "string", repo_id,
                                    "string", file_path,
                                    "string", parent_dir,
                                    "string", file_name,
                                    "string", user);
}

char *
wingufile_post_multi_files (SearpcClient *client,
                          const char *repo_id,
                          const char *parent_dir,
                          const char *filenames_json,
                          const char *paths_json,
                          const char *user,
                          GError **error)
{
    return searpc_client_call__string (client, "wingufile_post_multi_files", error,
                                       5, "string", repo_id,
                                       "string", parent_dir,
                                       "string", filenames_json,
                                       "string", paths_json,
                                       "string", user);
}

int
wingufile_set_user_quota (SearpcClient *client,
                        const char *user,
                        gint64 quota,
                        GError **error)
{
    return searpc_client_call__int (client, "set_user_quota", error,
                                    2, "string", user, "int64", &quota);
}

int
wingufile_set_org_quota (SearpcClient *client,
                       int org_id,
                       gint64 quota,
                       GError **error)
{
    return searpc_client_call__int (client, "set_org_quota", error,
                                    2, "int", org_id, "int64", &quota);
}

int
wingufile_set_org_user_quota (SearpcClient *client,
                            int org_id,
                            const char *user,
                            gint64 quota,
                            GError **error)
{
    return searpc_client_call__int (client, "set_org_user_quota", error,
                                    3, "int", org_id,
                                    "string", user,
                                    "int64", &quota);
}

int
wingufile_check_quota (SearpcClient *client,
                     const char *repo_id,
                     GError **error)
{
    return searpc_client_call__int (client, "check_quota", error,
                                    1, "string", repo_id);
}

int
wingufile_disable_auto_sync_async (SearpcClient *client,
                                 AsyncCallback callback,
                                 void *user_data)
{
    return searpc_client_async_call__int (client,
                                          "wingufile_disable_auto_sync",
                                          callback, user_data, 0);
}

int
wingufile_enable_auto_sync_async (SearpcClient *client,
                                 AsyncCallback callback,
                                 void *user_data)
{
    return searpc_client_async_call__int (client,
                                          "wingufile_enable_auto_sync",
                                          callback, user_data, 0);
}

int
wingufile_is_auto_sync_enabled_async (SearpcClient *client,
                                    AsyncCallback callback,
                                    void *user_data)
{
    return searpc_client_async_call__int (client,
                                          "wingufile_is_auto_sync_enabled",
                                          callback, user_data, 0);
}
