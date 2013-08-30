
import ccnet
from pysearpc import searpc_func, SearpcError

class SeafileRpcClient(ccnet.RpcClientBase):
    """RPC used in client"""

    def __init__(self, ccnet_client_pool, *args, **kwargs):
        ccnet.RpcClientBase.__init__(self, ccnet_client_pool, "wingufile-rpcserver",
                                     *args, **kwargs)

    @searpc_func("object", [])
    def wingufile_get_session_info():
        pass
    get_session_info = wingufile_get_session_info

    @searpc_func("int", ["string"])
    def wingufile_calc_dir_size(path):
        pass
    calc_dir_size = wingufile_calc_dir_size

    @searpc_func("int64", [])
    def wingufile_get_total_block_size():
        pass
    get_total_block_size = wingufile_get_total_block_size;

    @searpc_func("string", ["string"])
    def wingufile_get_config(key):
        pass
    get_config = wingufile_get_config

    @searpc_func("int", ["string", "string"])
    def wingufile_set_config(key, value):
        pass
    set_config = wingufile_set_config

    @searpc_func("int", ["string"])
    def wingufile_get_config_int(key):
        pass
    get_config_int = wingufile_get_config_int

    @searpc_func("int", ["string", "int"])
    def wingufile_set_config_int(key, value):
        pass
    set_config_int = wingufile_set_config_int

    @searpc_func("int", ["int"])
    def wingufile_set_upload_rate_limit(limit):
        pass
    set_upload_rate_limit = wingufile_set_upload_rate_limit

    @searpc_func("int", ["int"])
    def wingufile_set_download_rate_limit(limit):
        pass
    set_download_rate_limit = wingufile_set_download_rate_limit

    ### repo
    @searpc_func("objlist", ["int", "int"])
    def wingufile_get_repo_list():
        pass
    get_repo_list = wingufile_get_repo_list

    @searpc_func("object", ["string"])
    def wingufile_get_repo():
        pass
    get_repo = wingufile_get_repo

    @searpc_func("string", ["string", "string", "string", "string", "string", "int"])
    def wingufile_create_repo(name, desc, passwd, base, relay_id, keep_history):
        pass
    create_repo = wingufile_create_repo

    @searpc_func("int", ["string"])
    def wingufile_destroy_repo(repo_id):
        pass
    remove_repo = wingufile_destroy_repo

    @searpc_func("objlist", ["string", "string", "string"])
    def wingufile_diff():
        pass
    get_diff = wingufile_diff

    @searpc_func("object", ["string"])
    def wingufile_get_commit(commit_id):
        pass
    get_commit = wingufile_get_commit

    @searpc_func("objlist", ["string", "int", "int"])
    def wingufile_get_commit_list():
        pass
    get_commit_list = wingufile_get_commit_list

    @searpc_func("objlist", ["string", "int", "int"])
    def wingufile_list_dir(dir_id, offset, limit):
        pass
    list_dir = wingufile_list_dir

    @searpc_func("objlist", ["string", "string"])
    def wingufile_list_dir_by_path(commit_id, path):
        pass
    list_dir_by_path = wingufile_list_dir_by_path

    @searpc_func("string", ["string", "string"])
    def wingufile_get_dirid_by_path(commit_id, path):
        pass
    get_dirid_by_path = wingufile_get_dirid_by_path

    @searpc_func("objlist", ["string"])
    def wingufile_branch_gets(repo_id):
        pass
    branch_gets = wingufile_branch_gets

    @searpc_func("int", ["string", "string"])
    def wingufile_branch_add(repo_id, branch):
        pass
    branch_add = wingufile_branch_add

    ##### clone related
    @searpc_func("string", ["string", "string"])
    def gen_default_worktree(worktree_parent, repo_name):
        pass

    @searpc_func("string", ["string", "string", "string", "string", "string", "string", "string", "string", "string"])
    def wingufile_clone(repo_id, peer_id, repo_name, worktree, token, password, magic, peer_addr, peer_port, email):
        pass
    clone = wingufile_clone

    @searpc_func("string", ["string", "string", "string", "string", "string", "string", "string", "string", "string"])
    def wingufile_download(repo_id, peer_id, repo_name, wt_parent, token, password, magic, peer_addr, peer_port, email):
        pass
    download = wingufile_download

    @searpc_func("int", ["string"])
    def wingufile_cancel_clone_task(repo_id):
        pass
    cancel_clone_task = wingufile_cancel_clone_task

    @searpc_func("int", ["string"])
    def wingufile_remove_clone_task(repo_id):
        pass
    remove_clone_task = wingufile_remove_clone_task

    @searpc_func("objlist", [])
    def wingufile_get_clone_tasks():
        pass
    get_clone_tasks = wingufile_get_clone_tasks

    @searpc_func("object", ["string"])
    def wingufile_find_transfer_task(repo_id):
        pass
    find_transfer_task = wingufile_find_transfer_task
 
    @searpc_func("object", ["string"])
    def wingufile_get_checkout_task(repo_id):
        pass
    get_checkout_task = wingufile_get_checkout_task
    
    ### sync
    @searpc_func("int", ["string", "string"])
    def wingufile_sync(repo_id, peer_id):
        pass
    sync = wingufile_sync

    @searpc_func("object", ["string"])
    def wingufile_get_repo_sync_task():
        pass
    get_repo_sync_task = wingufile_get_repo_sync_task

    @searpc_func("object", ["string"])
    def wingufile_get_repo_sync_info():
        pass
    get_repo_sync_info = wingufile_get_repo_sync_info

    @searpc_func("int", [])
    def wingufile_is_auto_sync_enabled():
        pass
    is_auto_sync_enabled = wingufile_is_auto_sync_enabled

    ###### Property Management #########

    @searpc_func("int", ["string", "string"])
    def wingufile_set_repo_passwd(repo_id, passwd):
        pass
    set_repo_passwd = wingufile_set_repo_passwd

    @searpc_func("int", ["string", "string", "string"])
    def wingufile_set_repo_property(repo_id, key, value):
        pass
    set_repo_property = wingufile_set_repo_property

    @searpc_func("string", ["string", "string"])
    def wingufile_get_repo_property(repo_id, key):
        pass
    get_repo_property = wingufile_get_repo_property

    @searpc_func("string", ["string"])
    def wingufile_get_repo_relay_address(repo_id):
        pass
    get_repo_relay_address = wingufile_get_repo_relay_address

    @searpc_func("string", ["string"])
    def wingufile_get_repo_relay_port(repo_id):
        pass
    get_repo_relay_port = wingufile_get_repo_relay_port

    @searpc_func("int", ["string", "string", "string"])
    def wingufile_update_repo_relay_info(repo_id, addr, port):
        pass
    update_repo_relay_info = wingufile_update_repo_relay_info

    @searpc_func("int", ["string", "string"])
    def wingufile_set_repo_token(repo_id, token):
        pass
    set_repo_token = wingufile_set_repo_token

    @searpc_func("string", ["string"])
    def wingufile_get_repo_token(repo_id):
        pass
    get_repo_token = wingufile_get_repo_token


class SeafileThreadedRpcClient(ccnet.RpcClientBase):
    """RPC used in client that run in a thread"""

    def __init__(self, ccnet_client_pool, *args, **kwargs):
        ccnet.RpcClientBase.__init__(self, ccnet_client_pool, 
                                     "wingufile-threaded-rpcserver", 
                                     *args, **kwargs)

    @searpc_func("int", ["string", "string", "string"])
    def wingufile_edit_repo():
        pass
    edit_repo = wingufile_edit_repo

    @searpc_func("int", ["string", "string"])
    def wingufile_reset(repo_id, commit_id):
        pass
    reset = wingufile_reset

    @searpc_func("int", ["string", "string"])
    def wingufile_revert(repo_id, commit_id):
        pass
    revert = wingufile_revert

    @searpc_func("int", ["string", "string"])
    def wingufile_add(repo_id, path):
        pass
    add = wingufile_add

    @searpc_func("int", ["string", "string"])
    def wingufile_rm():
        pass
    rm = wingufile_rm

    @searpc_func("string", ["string", "string"])
    def wingufile_commit(repo_id, description):
        pass
    commit = wingufile_commit


class MonitorRpcClient(ccnet.RpcClientBase):

    def __init__(self, ccnet_client_pool):
        ccnet.RpcClientBase.__init__(self, ccnet_client_pool, "monitor-rpcserver")

    @searpc_func("int", ["string"])
    def monitor_get_repos_size(repo_ids):
        pass
    get_repos_size = monitor_get_repos_size


class SeafServerRpcClient(ccnet.RpcClientBase):

    def __init__(self, ccnet_client_pool, *args, **kwargs):
        ccnet.RpcClientBase.__init__(self, ccnet_client_pool, "seafserv-rpcserver",
                                     *args, **kwargs)

    # token for web access to repo
    @searpc_func("string", ["string", "string", "string", "string"])
    def wingufile_web_get_access_token(repo_id, obj_id, op, username):
        pass
    web_get_access_token = wingufile_web_get_access_token
    
    @searpc_func("object", ["string"])
    def wingufile_web_query_access_token(token):
        pass
    web_query_access_token = wingufile_web_query_access_token

    ###### GC    ####################
    @searpc_func("int", [])
    def wingufile_gc():
        pass
    gc = wingufile_gc

    @searpc_func("int", [])
    def wingufile_gc_get_progress():
        pass
    gc_get_progress = wingufile_gc_get_progress

    # password management
    @searpc_func("int", ["string", "string"])
    def wingufile_is_passwd_set(repo_id, user):
        pass
    is_passwd_set = wingufile_is_passwd_set

    @searpc_func("object", ["string", "string"])
    def wingufile_get_decrypt_key(repo_id, user):
        pass
    get_decrypt_key = wingufile_get_decrypt_key
    
class SeafServerThreadedRpcClient(ccnet.RpcClientBase):

    def __init__(self, ccnet_client_pool, *args, **kwargs):
        ccnet.RpcClientBase.__init__(self, ccnet_client_pool,
                                     "seafserv-threaded-rpcserver",
                                     *args, **kwargs)

    # repo manipulation 
    @searpc_func("string", ["string", "string", "string", "string"])
    def wingufile_create_repo(name, desc, owner_email, passwd):
        pass
    create_repo = wingufile_create_repo

    @searpc_func("object", ["string"])
    def wingufile_get_repo(repo_id):
        pass
    get_repo = wingufile_get_repo

    @searpc_func("int", ["string"])
    def wingufile_destroy_repo(repo_id):
        pass
    remove_repo = wingufile_destroy_repo

    @searpc_func("objlist", ["int", "int"])
    def wingufile_get_repo_list(start, limit):
        pass
    get_repo_list = wingufile_get_repo_list

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_edit_repo(repo_id, name, description, user):
        pass
    edit_repo = wingufile_edit_repo

    @searpc_func("int", ["string", "string"])
    def wingufile_is_repo_owner(user_id, repo_id):
        pass
    is_repo_owner = wingufile_is_repo_owner

    @searpc_func("string", ["string"])
    def wingufile_get_repo_owner(repo_id):
        pass
    get_repo_owner = wingufile_get_repo_owner
    
    @searpc_func("objlist", ["string"])
    def wingufile_list_owned_repos(user_id):
        pass
    list_owned_repos = wingufile_list_owned_repos

    @searpc_func("int64", ["string"])
    def wingufile_server_repo_size(repo_id):
        pass
    server_repo_size = wingufile_server_repo_size
    
    @searpc_func("int", ["string", "string"])
    def wingufile_repo_set_access_property(repo_id, role):
        pass
    repo_set_access_property = wingufile_repo_set_access_property
    
    @searpc_func("string", ["string"])
    def wingufile_repo_query_access_property(repo_id):
        pass
    repo_query_access_property = wingufile_repo_query_access_property

    @searpc_func("int",  ["string", "string", "string"])
    def wingufile_revert_on_server(repo_id, commit_id, user_name):
        pass
    revert_on_server = wingufile_revert_on_server

    @searpc_func("objlist", ["string", "string", "string"])
    def wingufile_diff():
        pass
    get_diff = wingufile_diff

    @searpc_func("int", ["string", "string", "string", "string", "string"])
    def wingufile_post_file(repo_id, tmp_file_path, parent_dir, filename, user):
        pass
    post_file = wingufile_post_file 

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_post_dir(repo_id, parent_dir, new_dir_name, user):
        pass
    post_dir = wingufile_post_dir 

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_post_empty_file(repo_id, parent_dir, filename, user):
        pass
    post_empty_file = wingufile_post_empty_file

    @searpc_func("int", ["string", "string", "string", "string", "string", "string"])
    def wingufile_put_file(repo_id, tmp_file_path, parent_dir, filename, user, head_id):
        pass
    put_file = wingufile_put_file 

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_del_file(repo_id, parent_dir, filename, user):
        pass
    del_file = wingufile_del_file 

    @searpc_func("int", ["string", "string", "string", "string", "string", "string", "string"])
    def wingufile_copy_file(src_repo, src_dir, src_filename, dst_repo, dst_dir, dst_filename, user):
        pass
    copy_file = wingufile_copy_file 

    @searpc_func("int", ["string", "string", "string", "string", "string", "string", "string"])
    def wingufile_move_file(src_repo, src_dir, src_filename, dst_repo, dst_dir, dst_filename, user):
        pass
    move_file = wingufile_move_file

    @searpc_func("int", ["string", "string", "string", "string", "string"])
    def wingufile_rename_file(repo_id, parent_dir, oldname, newname, user):
        pass
    rename_file = wingufile_rename_file 

    @searpc_func("int", ["string", "string"])
    def wingufile_is_valid_filename(repo_id, filename):
        pass
    is_valid_filename = wingufile_is_valid_filename 

    @searpc_func("object", ["string"])
    def wingufile_get_commit(commit_id):
        pass
    get_commit = wingufile_get_commit

    @searpc_func("objlist", ["string"])
    def wingufile_list_dir(dir_id):
        pass
    list_dir = wingufile_list_dir

    @searpc_func("int64", ["string"])
    def wingufile_get_file_size(file_id):
        pass
    get_file_size = wingufile_get_file_size

    @searpc_func("int64", ["string"])
    def wingufile_get_dir_size(dir_id):
        pass
    get_dir_size = wingufile_get_dir_size

    @searpc_func("objlist", ["string", "string"])
    def wingufile_list_dir_by_path(commit_id, path):
        pass
    list_dir_by_path = wingufile_list_dir_by_path

    @searpc_func("string", ["string", "string"])
    def wingufile_get_dirid_by_path(commit_id, path):
        pass
    get_dirid_by_path = wingufile_get_dirid_by_path

    @searpc_func("string", ["string", "string"])
    def wingufile_get_file_id_by_path(repo_id, path):
        pass
    get_file_id_by_path = wingufile_get_file_id_by_path

    @searpc_func("string", ["string", "string"])
    def wingufile_get_dir_id_by_path(repo_id, path):
        pass
    get_dir_id_by_path = wingufile_get_dir_id_by_path

    @searpc_func("string", ["string", "string"])
    def wingufile_get_file_id_by_commit_and_path(commit_id, path):
        pass
    get_file_id_by_commit_and_path = wingufile_get_file_id_by_commit_and_path

    @searpc_func("objlist", ["string", "string", "int", "int"])
    def wingufile_list_file_revisions(repo_id, path, max_revision, limit):
        pass
    list_file_revisions = wingufile_list_file_revisions

    @searpc_func("objlist", ["string", "string"])
    def wingufile_calc_files_last_modified(repo_id, parent_dir, limit):
        pass
    calc_files_last_modified = wingufile_calc_files_last_modified

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_revert_file(repo_id, commit_id, path, user):
        pass
    revert_file = wingufile_revert_file

    @searpc_func("int", ["string", "string", "string", "string"])
    def wingufile_revert_dir(repo_id, commit_id, path, user):
        pass
    revert_dir = wingufile_revert_dir

    @searpc_func("objlist", ["string", "int"])
    def get_deleted(repo_id, show_days):
        pass

    # share repo to user
    @searpc_func("string", ["string", "string", "string", "string"])
    def wingufile_add_share(repo_id, from_email, to_email, permission):
        pass
    add_share = wingufile_add_share

    @searpc_func("objlist", ["string", "string", "int", "int"])
    def wingufile_list_share_repos(email, query_col, start, limit):
        pass
    list_share_repos = wingufile_list_share_repos

    @searpc_func("objlist", ["int", "string", "string", "int", "int"])
    def wingufile_list_org_share_repos(org_id, email, query_col, start, limit):
        pass
    list_org_share_repos = wingufile_list_org_share_repos

    @searpc_func("int", ["string", "string", "string"])
    def wingufile_remove_share(repo_id, from_email, to_email):
        pass
    remove_share = wingufile_remove_share

    @searpc_func("int", ["string", "string", "string", "string"])
    def set_share_permission(repo_id, from_email, to_email, permission):
        pass

    # share repo to group
    @searpc_func("int", ["string", "int", "string", "string"])
    def wingufile_group_share_repo(repo_id, group_id, user_name, permisson):
        pass
    group_share_repo = wingufile_group_share_repo
    
    @searpc_func("int", ["string", "int", "string"])
    def wingufile_group_unshare_repo(repo_id, group_id, user_name):
        pass
    group_unshare_repo = wingufile_group_unshare_repo

    @searpc_func("string", ["string"])
    def wingufile_get_shared_groups_by_repo(repo_id):
        pass
    get_shared_groups_by_repo=wingufile_get_shared_groups_by_repo
    
    @searpc_func("string", ["int"])
    def wingufile_get_group_repoids(group_id):
        pass
    get_group_repoids = wingufile_get_group_repoids

    @searpc_func("objlist", ["string"])
    def get_group_repos_by_owner(user_name):
        pass

    @searpc_func("string", ["string"])
    def get_group_repo_owner(repo_id):
        pass

    @searpc_func("int", ["int", "string"])
    def wingufile_remove_repo_group(group_id, user_name):
        pass
    remove_repo_group = wingufile_remove_repo_group

    @searpc_func("int", ["int", "string", "string"])
    def set_group_repo_permission(group_id, repo_id, permission):
        pass
    
    # branch and commit
    @searpc_func("objlist", ["string"])
    def wingufile_branch_gets(repo_id):
        pass
    branch_gets = wingufile_branch_gets

    @searpc_func("objlist", ["string", "int", "int"])
    def wingufile_get_commit_list(repo_id, offset, limit):
        pass
    get_commit_list = wingufile_get_commit_list


    ###### Token ####################

    @searpc_func("int", ["string", "string", "string"])
    def wingufile_set_repo_token(repo_id, email, token):
        pass
    set_repo_token = wingufile_set_repo_token

    @searpc_func("string", ["string", "string"])
    def wingufile_get_repo_token_nonnull(repo_id, email):
        """Get the token of the repo for the email user. If the token does not
        exist, a new one is generated and returned.

        """
        pass
    get_repo_token_nonnull = wingufile_get_repo_token_nonnull

    
    @searpc_func("string", ["string", "string"])
    def wingufile_generate_repo_token(repo_id, email):
        pass
    generate_repo_token = wingufile_generate_repo_token

    @searpc_func("int", ["string", "string"])
    def wingufile_delete_repo_token(repo_id, token, user):
        pass
    delete_repo_token = wingufile_delete_repo_token
    
    @searpc_func("objlist", ["string"])
    def wingufile_list_repo_tokens(repo_id):
        pass
    list_repo_tokens = wingufile_list_repo_tokens

    @searpc_func("objlist", ["string"])
    def wingufile_list_repo_tokens_by_email(email):
        pass
    list_repo_tokens_by_email = wingufile_list_repo_tokens_by_email


    ###### quota ##########
    @searpc_func("int64", ["string"])
    def wingufile_get_user_quota_usage(user_id):
        pass
    get_user_quota_usage = wingufile_get_user_quota_usage

    @searpc_func("int64", ["string"])
    def wingufile_get_user_share_usage(user_id):
        pass
    get_user_share_usage = wingufile_get_user_share_usage

    @searpc_func("int64", ["int"])
    def wingufile_get_org_quota_usage(org_id):
        pass
    get_org_quota_usage = wingufile_get_org_quota_usage

    @searpc_func("int64", ["int", "string"])
    def wingufile_get_org_user_quota_usage(org_id, user):
        pass
    get_org_user_quota_usage = wingufile_get_org_user_quota_usage

    @searpc_func("int", ["string", "int64"])
    def set_user_quota(user, quota):
        pass

    @searpc_func("int64", ["string"])
    def get_user_quota(user):
        pass

    @searpc_func("int", ["int", "int64"])
    def set_org_quota(org_id, quota):
        pass

    @searpc_func("int64", ["int"])
    def get_org_quota(org_id):
        pass

    @searpc_func("int", ["int", "string", "int64"])
    def set_org_user_quota(org_id, user, quota):
        pass

    @searpc_func("int64", ["int", "string"])
    def get_org_user_quota(org_id, user):
        pass

    @searpc_func("int", ["string"])
    def check_quota(repo_id):
        pass

    # password management
    @searpc_func("int", ["string", "string", "string"])
    def wingufile_set_passwd(repo_id, user, passwd):
        pass
    set_passwd = wingufile_set_passwd

    @searpc_func("int", ["string", "string"])
    def wingufile_unset_passwd(repo_id, user, passwd):
        pass
    unset_passwd = wingufile_unset_passwd
    
    # repo permission checking
    @searpc_func("string", ["string", "string"])
    def check_permission(repo_id, user):
        pass

    # org repo
    @searpc_func("string", ["string", "string", "string", "string", "int"])
    def wingufile_create_org_repo(name, desc, user, passwd, org_id):
        pass
    create_org_repo = wingufile_create_org_repo

    @searpc_func("int", ["string"])
    def wingufile_get_org_id_by_repo_id(repo_id):
        pass
    get_org_id_by_repo_id = wingufile_get_org_id_by_repo_id    

    @searpc_func("objlist", ["int", "int", "int"])
    def wingufile_get_org_repo_list(org_id, start, limit):
        pass
    get_org_repo_list = wingufile_get_org_repo_list

    @searpc_func("int", ["int"])
    def wingufile_remove_org_repo_by_org_id(org_id):
        pass
    remove_org_repo_by_org_id = wingufile_remove_org_repo_by_org_id

    @searpc_func("objlist", ["int", "string"])
    def list_org_repos_by_owner(org_id, user):
        pass

    @searpc_func("string", ["string"])
    def get_org_repo_owner(repo_id):
        pass
    
    # org group repo
    @searpc_func("int", ["string", "int", "int", "string", "string"])
    def add_org_group_repo(repo_id, org_id, group_id, owner, permission):
        pass

    @searpc_func("int", ["string", "int", "int"])
    def del_org_group_repo(repo_id, org_id, group_id):
        pass

    @searpc_func("string", ["int", "int"])
    def get_org_group_repoids(org_id, group_id):
        pass

    @searpc_func("string", ["int", "int", "string"])
    def get_org_group_repo_owner(org_id, group_id, repo_id):
        pass

    @searpc_func("objlist", ["int", "string"])
    def get_org_group_repos_by_owner(org_id, user):
        pass

    @searpc_func("string", ["int", "string"])
    def get_org_groups_by_repo(org_id, repo_id):
        pass

    @searpc_func("int", ["int", "int", "string", "string"])
    def set_org_group_repo_permission(org_id, group_id, repo_id, permission):
        pass
    
    # inner pub repo
    @searpc_func("int", ["string", "string"])
    def set_inner_pub_repo(repo_id, permission):
        pass

    @searpc_func("int", ["string"])
    def unset_inner_pub_repo(repo_id):
        pass

    @searpc_func("objlist", [])
    def list_inner_pub_repos():
        pass

    @searpc_func("objlist", ["string"])
    def list_inner_pub_repos_by_owner(user):
        pass

    @searpc_func("int64", [])
    def count_inner_pub_repos():
        pass
    
    @searpc_func("int", ["string"])
    def is_inner_pub_repo(repo_id):
        pass

    # org inner pub repo
    @searpc_func("int", ["int", "string", "string"])
    def set_org_inner_pub_repo(org_id, repo_id, permission):
        pass

    @searpc_func("int", ["int", "string"])
    def unset_org_inner_pub_repo(org_id, repo_id):
        pass

    @searpc_func("objlist", ["int"])
    def list_org_inner_pub_repos(org_id):
        pass

    @searpc_func("objlist", ["int", "string"])
    def list_org_inner_pub_repos_by_owner(org_id, user):
        pass

    @searpc_func("int", ["string", "int"])
    def set_repo_history_limit(repo_id, days):
        pass

    @searpc_func("int", ["string"])
    def get_repo_history_limit(repo_id):
        pass

    # virtual repo
    @searpc_func("string", ["string", "string", "string", "string", "string"])
    def create_virtual_repo(origin_repo_id, path, repo_name, repo_desc, owner):
        pass

    @searpc_func("objlist", ["string"])
    def get_virtual_repos_by_owner(owner):
        pass

    @searpc_func("object", ["string", "string", "string"])
    def get_virtual_repo(origin_repo, path, owner):
        pass
