/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SHARE_MGR_H
#define SHARE_MGR_H

#include <glib.h>

struct _SeafileSession;

typedef struct _SeafShareManager SeafShareManager;
typedef struct _SeafShareManagerPriv SeafShareManagerPriv;
typedef struct _ShareRepoInfo ShareRepoInfo;

struct _SeafShareManager {
    struct _SeafileSession *winguf;

};

SeafShareManager*
winguf_share_manager_new (struct _SeafileSession *winguf);

int
winguf_share_manager_start (SeafShareManager *mgr);

int
winguf_share_manager_add_share (SeafShareManager *mgr, const char *repo_id,
                              const char *from_email, const char *to_email,
                              const char *permission);

int
winguf_share_manager_set_permission (SeafShareManager *mgr, const char *repo_id,
                                   const char *from_email, const char *to_email,
                                   const char *permission);

GList*
winguf_share_manager_list_share_repos (SeafShareManager *mgr, const char *email,
                                     const char *type, int start, int limit);

GList*
winguf_share_manager_list_org_share_repos (SeafShareManager *mgr,
                                         int org_id,
                                         const char *email,
                                         const char *type,
                                         int start, int limit);

GList *
winguf_share_manager_list_shared_to (SeafShareManager *mgr,
                                   const char *owner,
                                   const char *repo_id);

int
winguf_share_manager_remove_share (SeafShareManager *mgr, const char *repo_id,
                                 const char *from_email, const char *to_email);

/* Remove all share info of a repo. */
int
winguf_share_manager_remove_repo (SeafShareManager *mgr, const char *repo_id);

char *
winguf_share_manager_check_permission (SeafShareManager *mgr,
                                     const char *repo_id,
                                     const char *email);

#endif /* SHARE_MGR_H */

