import ccnet
import wingufile

pool = ccnet.ClientPool("basic/conf1")
wingufile_rpc = wingufile.RpcClient(pool)

repos = wingufile_rpc.get_repo_list("", 100)
for repo in repos:
    print repo

