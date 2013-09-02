#ifndef PASSWD_MGR_H
#define PASSWD_MGR_H

#include <glib.h>

struct _WingufileSession;
struct _SeafPasswdManagerPriv;
struct _WingufileCryptKey;

struct _SeafPasswdManager {
    struct _WingufileSession *session;
    struct _SeafPasswdManagerPriv *priv;
};
typedef struct _SeafPasswdManager SeafPasswdManager;

SeafPasswdManager *
winguf_passwd_manager_new (struct _WingufileSession *session);

int
winguf_passwd_manager_start (SeafPasswdManager *mgr);

/**
 * Set @passwd for @user to access contents of @repo_id.
 * This function:
 * 1. check whether @passwd is correct;
 * 2. calculate and store decryption key based on @passwd in memory.
 *
 * Returns 0 if @passwd is correct, -1 otherwise.
 */
int
winguf_passwd_manager_set_passwd (SeafPasswdManager *mgr,
                                const char *repo_id,
                                const char *user,
                                const char *passwd,
                                GError **error);

/**
 * Returns 0 if successfully unset user password, -1 otherwise.
 */
int
winguf_passwd_manager_unset_passwd (SeafPasswdManager *mgr,
                                  const char *repo_id,
                                  const char *user,
                                  GError **error);

/**
 * Check whether correct passwd has been set for @user
 * to access @repo_id.
 */
gboolean
winguf_passwd_manager_is_passwd_set (SeafPasswdManager *mgr,
                                   const char *repo_id,
                                   const char *user);

/**
 * Returns decryption key for @repo_id, NULL if it's not set.
 */
struct _WingufileCryptKey *
winguf_passwd_manager_get_decrypt_key (SeafPasswdManager *mgr,
                                     const char *repo_id,
                                     const char *user);

int
winguf_passwd_manager_get_decrypt_key_raw (SeafPasswdManager *mgr,
                                         const char *repo_id,
                                         const char *user,
                                         unsigned char *key_out,
                                         unsigned char *iv_out);

char *
winguf_passwd_manager_get_repo_passwd (SeafPasswdManager *mgr,
                                     const char *repo_id,
                                     const char *user);

#endif
