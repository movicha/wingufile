/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include "wingufile-session.h"
#include "repo-mgr.h"
#include "sync-mgr.h"
#include "utils.h"

#include "sync-repo-proc.h"
#include "sync-repo-common.h"

#define DEBUG_FLAG WINGUFILE_DEBUG_SYNC
#include "log.h"

/* 

   client                                           relay

            sync-repo-slave <repo-id> <branch>
            ---------------------------------->
            300 <head_commit>
            <----------------------------------

        or
            
            301 No such repo
            <----------------------------------

            302 No such branch
            <----------------------------------
            
*/


G_DEFINE_TYPE (WingufileSyncRepoProc, wingufile_sync_repo_proc, CCNET_TYPE_PROCESSOR)


static int
sync_repo_start (CcnetProcessor *processor, int argc, char **argv);

static void
handle_response (CcnetProcessor *processor,
                 char *code, char *code_msg,
                 char *content, int clen);

static void
wingufile_sync_repo_proc_class_init (WingufileSyncRepoProcClass *klass)
{
    CcnetProcessorClass *proc_class = CCNET_PROCESSOR_CLASS (klass);

    proc_class->name = "wingufile-sync-repo";
    proc_class->start = sync_repo_start;
    proc_class->handle_response = handle_response;
}

static void
wingufile_sync_repo_proc_init (WingufileSyncRepoProc *processor)
{
}


static int
sync_repo_start (CcnetProcessor *processor, int argc, char **argv)
{
    WingufileSyncRepoProc *proc = (WingufileSyncRepoProc *) processor;

    if (argc != 0) {
        winguf_warning ("[sync-repo] argc should be 0.\n");
        ccnet_processor_done (processor, FALSE);
        return 0;
    }

    if (!proc->task) {
        winguf_warning ("[sync-repo] Error: not provide info task.\n");
        ccnet_processor_done (processor, FALSE);
        return 0;
    }

    char buf[256];

    /* Use a virutal "fetch_head" branch that works both on client and server. */
    snprintf (buf, 256, "remote %s wingufile-sync-repo-slave %s %s",
              processor->peer_id, proc->task->info->repo_id, "fetch_head");
    
    ccnet_processor_send_request (processor, buf);

    return 0;
}


static void handle_response (CcnetProcessor *processor,
                             char *code, char *code_msg,
                             char *content, int clen)
{
    WingufileSyncRepoProc *proc = (WingufileSyncRepoProc *)processor;

    proc->task->info->deleted_on_relay = FALSE;
    proc->task->info->branch_deleted_on_relay = FALSE;
    proc->task->info->repo_corrupted = FALSE;

    if (memcmp (code, SC_COMMIT_ID, 3) == 0) {
        
        if (content[clen-1] != '\0') {
            winguf_warning ("[sync-repo] Response not end with NULL\n");
            ccnet_processor_done (processor, FALSE);
            return;
        }

        /* g_debug ("[sync-repo] Get repo head commit %s\n", content); */
        if (strlen(content) != 40) {
            winguf_debug ("[sync-repo] Invalid commit id\n");
            ccnet_processor_done (processor, FALSE);
            return;
        }

        memcpy(proc->task->info->head_commit, content, 41);

        ccnet_processor_done (processor, TRUE);
    } else if (memcmp (code, SC_NO_REPO, 3) == 0) {
        proc->task->info->deleted_on_relay = TRUE;
        ccnet_processor_done (processor, TRUE);
    } else if (memcmp (code, SC_NO_BRANCH, 3) == 0) {
        proc->task->info->branch_deleted_on_relay = TRUE;
        ccnet_processor_done (processor, TRUE);
    } else if (memcmp (code, SC_REPO_CORRUPT, 3) == 0) {
        proc->task->info->repo_corrupted = TRUE;
        ccnet_processor_done (processor, TRUE);
    } else 
        ccnet_processor_done (processor, FALSE);
}
