
import service
from service import ccnet_rpc, wingufile_rpc, \
    wingufile_threaded_rpc, monitor_rpc, applet_rpc
from service import get_peers_by_role, send_command, get_peers
from service import get_repos, get_repo, get_commits, \
    get_branches, open_dir, get_diff, list_dir, remove_repos_on_relay, \
    get_default_wingufile_worktree, get_current_prefs

from service import get_default_relay
from service import CCNET_CONF_PATH
from wingufile import TaskType

