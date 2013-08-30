#include <ccnet.h>
#include <ccnet/ccnetrpc-transport.h>
#include <wingufile.h>

#include "rpc-wrapper.h"
#include "applet-log.h"
#include "utils.h"
#include "wingufile-applet.h"

#include <searpc-client.h>


void 
applet_init_ccnet_rpc (CcnetClient *sync_client)
{
    applet->ccnet_rpc_client = ccnet_create_rpc_client (
        sync_client, NULL, "ccnet-rpcserver");
}

void 
applet_init_wingufile_rpc (CcnetClient *client)
{
    /* async searpc client, for invoking wingufile rpc */    
    applet->wingufile_rpc_client = ccnet_create_async_rpc_client (
        client, NULL, "wingufile-rpcserver");
}

int
call_wingufile_get_config (char *key, AsyncCallback callback, void *data)
{
    return wingufile_get_config_async (applet->wingufile_rpc_client, key,
                                     callback, data);
}

int
call_wingufile_set_config (char *key, char *value,
                         AsyncCallback callback, void *data)
{
    return wingufile_set_config_async (applet->wingufile_rpc_client, key, value,
                                     callback, data);
}

int call_wingufile_disable_auto_sync (AsyncCallback callback, void *data)
{
    return wingufile_disable_auto_sync_async (applet->wingufile_rpc_client, callback, data);
}

int call_wingufile_enable_auto_sync (AsyncCallback callback, void *data)
{
    return wingufile_enable_auto_sync_async (applet->wingufile_rpc_client, callback, data);
}
