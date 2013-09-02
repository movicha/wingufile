/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef WINGUFILE_CONFIG_H
#define WINGUFILE_CONFIG_H

#include "wingufile-session.h"
#include "db.h"

#define KEY_MONITOR_ID  "monitor_id"
#define KEY_CHECK_REPO_PERIOD "check_repo_period"
#define KEY_DB_HOST "db_host"
#define KEY_DB_USER "db_user"
#define KEY_DB_PASSWD "db_passwd"
#define KEY_DB_NAME "db_name"
#define KEY_UPLOAD_LIMIT "upload_limit"
#define KEY_DOWNLOAD_LIMIT "download_limit"

/*
 * Returns: config value in string. The string should be freed by caller. 
 */
char *
wingufile_session_config_get_string (WingufileSession *session,
                                   const char *key);

/*
 * Returns:
 * If key exists, @exists will be set to TRUE and returns the value;
 * otherwise, @exists will be set to FALSE and returns -1.
 */
int
wingufile_session_config_get_int (WingufileSession *session,
                                const char *key,
                                gboolean *exists);

int
wingufile_session_config_set_string (WingufileSession *session,
                                   const char *key,
                                   const char *value);

int
wingufile_session_config_set_int (WingufileSession *session,
                                const char *key,
                                int value);

sqlite3 *
wingufile_session_config_open_db (const char *db_path);

#endif
