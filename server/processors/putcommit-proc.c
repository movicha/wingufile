/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <ccnet.h>
#include "net.h"
#include "utils.h"

#include "wingufile-session.h"
#include "commit-mgr.h"
#include "putcommit-proc.h"
#include "processors/objecttx-common.h"
#include "object-list.h"
#include "vc-common.h"

typedef struct  {
    char        commit_id[41];
    char        object_path[SEAF_PATH_MAX];
    gboolean    transfer_started;
    int         fd;
} WingufilePutcommitProcPriv;

#define GET_PRIV(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), WINGUFILE_TYPE_PUTCOMMIT_PROC, WingufilePutcommitProcPriv))

#define USE_PRIV \
    WingufilePutcommitProcPriv *priv = GET_PRIV(processor);

static int put_commit_start (CcnetProcessor *processor, int argc, char **argv);
static void handle_update (CcnetProcessor *processor,
                           char *code, char *code_msg,
                           char *content, int clen);

static int send_commit_ids (CcnetProcessor *processor, const char *head);


G_DEFINE_TYPE (WingufilePutcommitProc, wingufile_putcommit_proc, CCNET_TYPE_PROCESSOR)

static void
wingufile_putcommit_proc_class_init (WingufilePutcommitProcClass *klass)
{
    CcnetProcessorClass *proc_class = CCNET_PROCESSOR_CLASS (klass);

    proc_class->name = "putcommit-proc";
    proc_class->start = put_commit_start;
    proc_class->handle_update = handle_update;

    g_type_class_add_private (klass, sizeof (WingufilePutcommitProcPriv));
}

static void
wingufile_putcommit_proc_init (WingufilePutcommitProc *processor)
{
}


static int
put_commit_start (CcnetProcessor *processor, int argc, char **argv)
{
    char *commit_id;
    char *session_token;
    USE_PRIV;

    if (argc != 2) {
        ccnet_processor_send_response (processor, SC_BAD_ARGS, SS_BAD_ARGS, NULL, 0);
        ccnet_processor_done (processor, FALSE);
        return -1;
    }

    session_token = argv[1];
    if (winguf_token_manager_verify_token (winguf->token_mgr,
                                         processor->peer_id,
                                         session_token, NULL) < 0) {
        ccnet_processor_send_response (processor, 
                                       SC_ACCESS_DENIED, SS_ACCESS_DENIED,
                                       NULL, 0);
        ccnet_processor_done (processor, FALSE);
        return -1;
    }

    commit_id = argv[0];

    memcpy (priv->commit_id, commit_id, 41);
    ccnet_processor_send_response (processor, SC_OK, SS_OK, NULL, 0);

    return send_commit_ids (processor, commit_id);
}

static gboolean
commit_collector (SeafCommit *commit, void *data, gboolean *stop)
{
    ObjectList *ol = data;

    object_list_insert (ol, commit->commit_id);

    return TRUE;
}

static int
send_commit_ids (CcnetProcessor *processor, const char *head)
{
    char buf[2048];
    char *ptr = buf;
    int i, count = 0;
    int ret;
    
    ObjectList *ol = object_list_new ();
    ret = winguf_commit_manager_traverse_commit_tree (winguf->commit_mgr,
                                                    head,
                                                    commit_collector,
                                                    ol, FALSE);
    if (ret == FALSE) {
        object_list_free (ol);
        g_warning ("[putcommit] Load commits error\n");
        ccnet_processor_send_response (
            processor, SC_NOT_FOUND, SS_NOT_FOUND, NULL, 0);
        ccnet_processor_done (processor, FALSE);
        return -1;
    }

    int ollen = object_list_length(ol);
    g_assert (ollen != 0);

    for (i = 0; i < ollen; i++) {
        memcpy (ptr, g_ptr_array_index(ol->obj_ids, i), 40);
        ptr += 40;
        *ptr++ = '\n';

        if (++count == 48) {
            *ptr = '\0';
            ccnet_processor_send_response (processor, SC_COMMIT_IDS, 
                                           SS_COMMIT_IDS, buf, 41 * count + 1);
            ptr = buf;
            count = 0;
        }
    }

    if (count) {
        *ptr = '\0';
        ccnet_processor_send_response (processor, SC_COMMIT_IDS, 
                                       SS_COMMIT_IDS, buf, 41 * count + 1);
    }

    ccnet_processor_send_response (processor, SC_END, SS_END, NULL, 0);

    return 0;
}


static gboolean
send_commit (CcnetProcessor *processor, char *object_id)
{
    char *data;
    int len;
    ObjectPack *pack = NULL;
    int pack_size;

    if (winguf_obj_store_read_obj (winguf->commit_mgr->obj_store,
                                 object_id, (void**)&data, &len) < 0) {
        g_warning ("Failed to read commit %s.\n", object_id);
        goto fail;
    }

    pack_size = sizeof(ObjectPack) + len;
    pack = malloc (pack_size);
    memcpy (pack->id, object_id, 41);
    memcpy (pack->object, data, len);

    ccnet_processor_send_response (processor, SC_OBJECT, SS_OBJECT,
                                   (char *)pack, pack_size);

    g_free (data);
    free (pack);
    return TRUE;

fail:
    ccnet_processor_send_response (processor, SC_NOT_FOUND, SS_NOT_FOUND,
                                   object_id, 41);
    ccnet_processor_done (processor, FALSE);
    return FALSE;
}

static void
send_commits (CcnetProcessor *processor, char *content, int clen)
{
    char *object_id;
    int n_objects;
    int i;

    if (clen % 41 != 1 || content[clen-1] != '\0') {
        g_warning ("[putcommit] Bad commit object list.\n");
        ccnet_processor_send_response (processor, SC_BAD_OL, SS_BAD_OL, NULL, 0);
        ccnet_processor_done (processor, FALSE);
        return;
    }

    n_objects = clen/41;

    object_id = content;
    for (i = 0; i < n_objects; ++i) {
        object_id[40] = '\0';
        if (send_commit (processor, object_id) == FALSE)
            return;

        object_id += 41;
    }
}

static void handle_update (CcnetProcessor *processor,
                           char *code, char *code_msg,
                           char *content, int clen)
{
    if (strncmp(code, SC_GET_OBJECT, 3) == 0) {
        send_commits (processor, content, clen);
    } else if (strncmp(code, SC_END, 3) == 0) {
        ccnet_processor_done (processor, TRUE);
    } else {
        g_warning ("[putcommit] Bad response: %s %s\n", code, code_msg);
        ccnet_processor_done (processor, FALSE);
    }
}
