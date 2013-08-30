/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_H
#define WINGUFILE_H

char *
wingufile_create_repo (SearpcClient *client,
                     const gchar *name, 
                     const gchar *description,
                     const gchar *worktree,
                     const gchar *passwd,
                     const gchar *relay_id,
                     int keep_local_history, GError **error);

int
wingufile_create_repo_async (SearpcClient *client,
                           const gchar *name, 
                           const gchar *description,
                           const gchar *worktree,
                           const gchar *passwd,
                           const gchar *relay_id,
                           int keep_local_history,
                           AsyncCallback callback, void *user_data);

int wingufile_destroy_repo (SearpcClient *client,
                          const char *repo_id, GError **error);

int wingufile_set_repo_token (SearpcClient *client,
                            const char *repo_id,
                            const char *token,
                            GError **error);

char *
wingufile_get_repo_token (SearpcClient *client,
                        const char *repo_id,
                        GError **error);


int
wingufile_set_repo_property (SearpcClient *client,
                           const char *repo_id,
                           const char *key,
                           const char *value,
                           GError **error);

GList *
wingufile_get_repo_list (SearpcClient *client,
                       int offset,
                       int limit, GError **error);

GObject *
wingufile_get_repo (SearpcClient *client,
                  const char *repo_id,
                  GError **error);


char *wingufile_get_config (SearpcClient *client, const char *key, GError **error);

int wingufile_get_config_async (SearpcClient *client, const char *key,
                              AsyncCallback callback, void *user_data);

int wingufile_set_config_async (SearpcClient *client,
                              const char *key, const char *value,
                              AsyncCallback callback, void *user_data);

int wingufile_calc_dir_size (SearpcClient *client, const char *path, GError **error);


/* server  */
int wingufile_add_chunk_server (SearpcClient *client, const char *server_id,
                              GError **error);
int wingufile_del_chunk_server (SearpcClient *client, const char *server_id,
                              GError **error);
char *wingufile_list_chunk_servers (SearpcClient *client, GError **error);
int wingufile_set_monitor (SearpcClient *client, const char *peer_id, GError **error);
char *wingufile_get_monitor (SearpcClient *client, GError **error);

char *
wingufile_repo_query_access_property (SearpcClient *client,
                                    const char *repo_id,
                                    GError **error);

GObject *
wingufile_web_query_access_token (SearpcClient *client,
                                const char *token,
                                GError **error);

GObject *
wingufile_get_decrypt_key (SearpcClient *client,
                         const char *repo_id,
                         const char *user,
                         GError **error);

char *
wingufile_put_file (SearpcClient *client,
                  const char *repo_id,
                  const char *file_path,
                  const char *parent_dir,
                  const char *file_name,
                  const char *user,
                  const char *head_id,
                  GError **error);

int
wingufile_post_file (SearpcClient *client,
                   const char *repo_id,
                   const char *file_path,
                   const char *parent_dir,
                   const char *file_name,
                   const char *user,
                   GError **error);

#define POST_FILE_ERR_FILENAME 401

char *
wingufile_post_multi_files (SearpcClient *client,
                          const char *repo_id,
                          const char *parent_dir,
                          const char *filenames_json,
                          const char *paths_json,
                          const char *user,
                          GError **error);

int
wingufile_set_user_quota (SearpcClient *client,
                        const char *user,
                        gint64 quota,
                        GError **error);

int
wingufile_set_org_quota (SearpcClient *client,
                       int org_id,
                       gint64 quota,
                       GError **error);

int
wingufile_set_org_user_quota (SearpcClient *client,
                            int org_id,
                            const char *user,
                            gint64 quota,
                            GError **error);

int
wingufile_check_quota (SearpcClient *client,
                     const char *repo_id,
                     GError **error);

int
wingufile_disable_auto_sync_async (SearpcClient *client,
                                 AsyncCallback callback,
                                 void *user_data);
int
wingufile_enable_auto_sync_async (SearpcClient *client,
                                AsyncCallback callback,
                                void *user_data);

int
wingufile_is_auto_sync_enabled_async (SearpcClient *client,
                                    AsyncCallback callback,
                                    void *user_data);

#endif
