/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SEAF_PERM_MGR_H
#define SEAF_PERM_MGR_H

#include <glib.h>

struct _WingufileSession;

typedef struct _SeafPermManager SeafPermManager;
typedef struct _SeafPermManagerPriv SeafPermManagerPriv;

struct _SeafPermManager {
    struct _WingufileSession *winguf;

    SeafPermManagerPriv *priv;
};

SeafPermManager*
winguf_perm_manager_new (struct _WingufileSession *winguf);

int
winguf_perm_manager_init (SeafPermManager *mgr);

int
winguf_perm_manager_set_repo_owner (SeafPermManager *mgr,
                                  const char *repo_id,
                                  const char *user_id);

char *
winguf_perm_manager_get_repo_owner (SeafPermManager *mgr,
                                  const char *repo_id);

/* TODO: add start and limit. */
/* Get repos owned by this user.
 */
GList *
winguf_perm_manager_get_repos_by_owner (SeafPermManager *mgr,
                                      const char *user_id);

#endif
