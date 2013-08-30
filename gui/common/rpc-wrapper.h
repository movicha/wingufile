#ifndef RPC_WRAPPER_H
#define RPC_WRAPPER_H 
#include <ccnet.h>

void applet_init_ccnet_rpc (CcnetClient *sync_client);
void applet_init_wingufile_rpc (CcnetClient *client);

/* ----------------------------------------
 * Functions making using of RPCS 
 * ----------------------------------------
 */

int
call_wingufile_get_config (char *key, AsyncCallback callback, void *data);

int
call_wingufile_set_config (char *key, char *value,
                         AsyncCallback callback, void *data);

int call_wingufile_disable_auto_sync (AsyncCallback callback, void *data);
int call_wingufile_enable_auto_sync (AsyncCallback callback, void *data);

#endif
