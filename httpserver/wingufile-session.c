#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ccnet.h>
#include <utils.h>
#include <locale.h>

#include "wingufile-session.h"
#include "wingufile-config.h"
#include "winguf-utils.h"


/* Zip filename in windows should be encoded in UTF-8 to be consistent across
 * all system encodings. However, WinRAR(a much popular compress software)
 * does not support UTF-8 filename.
 *
 * To sovle this problem, set the `windows_encoding` under the [zip] category
 * in wingufile.conf. If set, file name would be converted to the specified
 * encoding. Otherwise, the UTF-8 way would be used.
 */
void
load_zip_encoding_config (WingufileSession *session)
{
    char *encoding;
    GError *error = NULL;

    encoding = g_key_file_get_string (session->config, "zip", "windows_encoding", &error);
    if (encoding) {
        session->windows_encoding = encoding;
    } else {
        /* No windows specific encoding is specified. Set the ZIP_UTF8 flag. */
        setlocale (LC_ALL, "en_US.UTF-8");
    }
}

WingufileSession *
wingufile_session_new(const char *wingufile_dir,
                    CcnetClient *ccnet_session)
{
    char *abs_wingufile_dir;
    char *tmp_file_dir;
    char *config_file_path;
    struct stat st;
    GKeyFile *config;
    WingufileSession *session = NULL;

    if (!ccnet_session)
        return NULL;

    abs_wingufile_dir = ccnet_expand_path (wingufile_dir);
    tmp_file_dir = g_build_filename (abs_wingufile_dir, "tmpfiles", NULL);
    config_file_path = g_build_filename (abs_wingufile_dir, "wingufile.conf", NULL);

    if (g_lstat(abs_wingufile_dir, &st) < 0 || !S_ISDIR(st.st_mode)) {
        g_warning ("Wingufile data dir %s does not exist and is unable to create\n",
                   abs_wingufile_dir);
        goto onerror;
    }

    if (g_lstat(tmp_file_dir, &st) < 0 || !S_ISDIR(st.st_mode)) {
        g_warning ("Wingufile tmp dir %s does not exist and is unable to create\n",
                   tmp_file_dir);
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
    session->config = config;

    if (load_database_config (session) < 0) {
        g_warning ("Failed to load database config.\n");
        goto onerror;
    }
    load_zip_encoding_config (session);

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

    return session;

onerror:
    free (abs_wingufile_dir);
    g_free (tmp_file_dir);
    g_free (config_file_path);
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

    return 0;
}

int
wingufile_session_start (WingufileSession *session)
{
    return 0;
}
