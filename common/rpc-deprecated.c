
gint64
wingufile_repo_size(const char *repo_id, GError **error)
{
    SeafBranch *local;
    RepoSize *rs;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    local = winguf_branch_manager_get_branch (winguf->branch_mgr, repo_id, "local");
    if (!local) {
        g_set_error (error, WINGUFILE_DOMAIN, -1, "No local branch");
        return -1;
    }

    rs = winguf_info_manager_get_repo_size_from_db (winguf->info_mgr, repo_id);
    if (!rs) {
        g_set_error (error, WINGUFILE_DOMAIN, -1, "Database error");
        winguf_branch_unref (local);
        return -1;
    }

    //no new commit on "local" branch
    if (strcmp (rs->commit_id, local->commit_id) == 0) {
        winguf_branch_unref (local);
        gint64 size = rs->size;
        g_free (rs);
        return size;
    }

    gint64 size = winguf_branch_manager_calculate_branch_size (
        winguf->branch_mgr, repo_id, local->commit_id);
    winguf_branch_unref (local);
    g_free (rs);
    if (size < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, -1, "Calulate branch size error");
        return -1;
    }

    return size;
}

int
wingufile_branch_del (const char *repo_id, const char *branch_name, GError **error)
{
    SeafRepo *repo;
    SeafBranch *branch;

    if (!repo_id || !branch_name) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return -1;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such repository");
        return -1;
    }

    branch = winguf_branch_manager_get_branch (winguf->branch_mgr, repo->id, branch_name);
    if (!branch) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such branch");
        return -1;
    }

    if (strcmp(repo->head->name, branch->name) == 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS,
                     "Cannot delete the branch which you are currently on");
        return -1;
    }

    /*
     * TODO: we just delete branch itself and need to remove commits using gc.
     */
    winguf_repo_manager_branch_repo_unmap (winguf->repo_mgr, branch);
    winguf_branch_manager_del_branch (winguf->branch_mgr, repo->id, branch->name);
    winguf_branch_unref (branch);

    return 0;
}

const char *
wingufile_status (const char *repo_id, GError **error)
{
    SeafRepo *repo;
    char *res_str;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "No such repository");
        return NULL;
    }

    /* Return immediately if repo is locked, so that the GUI will not
     * be frozen.
     */
    if (pthread_mutex_trylock (&repo->lock) == EBUSY) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_REPO_LOCKED, "Repo is locked");
        return NULL;
    }

    res_str = winguf_repo_status (repo);
    if (!res_str) {
        g_set_error (error, WINGUFILE_DOMAIN, -1, "Failed to status");
        pthread_mutex_unlock (&repo->lock);
        return NULL;
    }

    pthread_mutex_unlock (&repo->lock);

    return res_str;
}

int
wingufile_set_repo_lantoken (const char *repo_id,
                           const char *token,
                           GError **error)
{
    int ret;

    if (repo_id == NULL || token == NULL) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return -1;
    }

    if (!winguf_repo_manager_repo_exists (winguf->repo_mgr, repo_id)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "Repo %s doesn't exist", repo_id);
        return -1;
    }

    ret = winguf_repo_manager_set_repo_lantoken (winguf->repo_mgr,
                                               repo_id, token);
    if (ret < 0) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_INTERNAL,
                     "Failed to set token for repo %s", repo_id);
        return -1;
    }

    return 0;
}

char *
wingufile_get_repo_lantoken (const char *repo_id,
                           GError **error)
{
    char *token;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Arguments should not be empty");
        return NULL;
    }

    token = winguf_repo_manager_get_repo_lantoken (winguf->repo_mgr, repo_id);
    if (!token)
        return g_strdup(DEFAULT_REPO_TOKEN);

    return token;
}

/* ------------------ Public RPC calls. ------------ */

static int check_repo_public (SeafRepo *repo)
{
    if (repo)
        return repo->net_browsable;

    return 0;
}

static int check_commit_public (SeafCommit *c)
{
    SeafRepo *repo;

    g_assert (c != NULL);

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, c->repo_id);
    if (repo != NULL || check_repo_public(repo))
        return 1;

    return 0;
}

static GList *
convert_pubrepo_list (GList *inner_repos)
{
    GList *ret = NULL, *ptr;

    for (ptr = inner_repos; ptr; ptr=ptr->next) {
        SeafRepo *r = ptr->data;
        if (!check_repo_public(r))
            continue;
        WingufileRepo *repo = wingufile_repo_new ();
        g_object_set (repo, "id", r->id, "name", r->name,
                      "desc", r->desc, "encrypted", r->encrypted,
                      NULL);

        g_object_set (repo,
                      /* "auto-sync", r->auto_sync, */
                      "relay-id", r->relay_id,
                      "auto-sync", r->auto_sync,
                      NULL);

        g_object_set (repo,
                      "last-modify", wingufile_repo_last_modify(r->id, NULL),
                      NULL);

        ret = g_list_prepend (ret, repo);
    }
    ret = g_list_reverse (ret);

    return ret;
}

GList* wingufile_get_repo_list_pub (int start, int limit, GError **error)
{
    /* TODO: use start_id and limit */
    GList *repos = winguf_repo_manager_get_repo_list(winguf->repo_mgr, start, limit);
    GList *ret = NULL;

    ret = convert_pubrepo_list (repos);
    g_list_free (repos);
    return ret;
}

GObject* wingufile_get_repo_pub (const gchar* id, GError **error)
{
    SeafRepo *r;

    r = winguf_repo_manager_get_repo (winguf->repo_mgr, id);
    if (r == NULL || !check_repo_public(r))
        return NULL;

    WingufileRepo *repo = wingufile_repo_new ();
    g_object_set (repo, "id", r->id, "name", r->name,
                  "desc", r->desc, "encrypted", r->encrypted,
                  "head_branch", r->head ? r->head->name : NULL,
                  NULL);

    g_object_set (repo,
                  /* "auto-sync", r->auto_sync, */
                  "relay-id", r->relay_id,
                  "auto-sync", r->auto_sync,
                  NULL);

    g_object_set (repo,
                  "last-modify", wingufile_repo_last_modify(r->id, NULL),
                  NULL);

    return (GObject *)repo;
}

GObject*
wingufile_get_commit_pub (const gchar *id, GError **error)
{
    WingufileCommit *commit;
    SeafCommit *c;

    c = winguf_commit_manager_get_commit (winguf->commit_mgr, id);
    if (!c || !check_commit_public(c))
        return NULL;

    commit = convert_to_wingufile_commit (c);
    winguf_commit_unref (c);
    return (GObject *)commit;
}

GList*
wingufile_get_commit_list_pub (const char *repo_id,
                             int offset,
                             int limit,
                             GError **error)
{
    SeafRepo *repo;

    /* correct parameter */
    if (offset < 0)
        offset = 0;

    if (!repo_id) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_ARGS, "Argument should not be null");
        return NULL;
    }

    repo = winguf_repo_manager_get_repo (winguf->repo_mgr, repo_id);
    if (!repo || !check_repo_public(repo)) {
        g_set_error (error, WINGUFILE_DOMAIN, SEAF_ERR_BAD_REPO, "No such repository");
        return NULL;
    }

    return wingufile_get_commit_list(repo_id, offset, limit, error);
}

GList *
wingufile_list_dir_pub (const char *dir_id, GError **error)
{
    return wingufile_list_dir (dir_id, error);
}
