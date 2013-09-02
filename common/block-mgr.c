/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include "wingufile-session.h"
#include "utils.h"
#include "winguf-utils.h"
#include "block-mgr.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <glib/gstdio.h>

#include "block-backend.h"

#define SEAF_BLOCK_DIR "blocks"


extern BlockBackend *
block_backend_fs_new (const char *block_dir, const char *tmp_dir);


SeafBlockManager *
winguf_block_manager_new (struct _WingufileSession *winguf,
                        const char *winguf_dir)
{
    SeafBlockManager *mgr;

    mgr = g_new0 (SeafBlockManager, 1);
    mgr->winguf = winguf;

#ifdef WINGUFILE_SERVER
    mgr->backend = load_block_backend(mgr->winguf->config);
#endif
    if (!mgr->backend) {
        char *block_dir;
        block_dir = g_build_filename (winguf_dir, SEAF_BLOCK_DIR, NULL);
        mgr->backend = block_backend_fs_new (block_dir, winguf->tmp_file_dir);
        g_free (block_dir);
        if (!mgr->backend) {
            g_warning ("[Block mgr] Failed to load backend.\n");
            goto onerror;
        }
    }

    return mgr;

onerror:
    g_free (mgr);

    return NULL;
}

int
winguf_block_manager_init (SeafBlockManager *mgr)
{
    return 0;
}


BlockHandle *
winguf_block_manager_open_block (SeafBlockManager *mgr,
                               const char *block_id,
                               int rw_type)
{
    return mgr->backend->open_block (mgr->backend, block_id, rw_type);
}

int
winguf_block_manager_read_block (SeafBlockManager *mgr,
                               BlockHandle *handle,
                               void *buf, int len)
{
    return mgr->backend->read_block (mgr->backend, handle, buf, len);
}

int
winguf_block_manager_write_block (SeafBlockManager *mgr,
                                BlockHandle *handle,
                                const void *buf, int len)
{
    return mgr->backend->write_block (mgr->backend, handle, buf, len);
}

int
winguf_block_manager_close_block (SeafBlockManager *mgr,
                                BlockHandle *handle)
{
    return mgr->backend->close_block (mgr->backend, handle);
}

void
winguf_block_manager_block_handle_free (SeafBlockManager *mgr,
                                      BlockHandle *handle)
{
    return mgr->backend->block_handle_free (mgr->backend, handle);
}

int
winguf_block_manager_commit_block (SeafBlockManager *mgr,
                                 BlockHandle *handle)
{
    return mgr->backend->commit_block (mgr->backend, handle);
}
    
gboolean winguf_block_manager_block_exists (SeafBlockManager *mgr,
                                          const char *block_id)
{
    return mgr->backend->exists (mgr->backend, block_id);
}

int
winguf_block_manager_remove_block (SeafBlockManager *mgr,
                                 const char *block_id)
{
    return mgr->backend->remove_block (mgr->backend, block_id);
}

BlockMetadata *
winguf_block_manager_stat_block (SeafBlockManager *mgr,
                               const char *block_id)
{
    return mgr->backend->stat_block (mgr->backend, block_id);
}

BlockMetadata *
winguf_block_manager_stat_block_by_handle (SeafBlockManager *mgr,
                                         BlockHandle *handle)
{
    return mgr->backend->stat_block_by_handle (mgr->backend, handle);
}

int
winguf_block_manager_foreach_block (SeafBlockManager *mgr,
                                  SeafBlockFunc process,
                                  void *user_data)
{
    return mgr->backend->foreach_block (mgr->backend, process, user_data);
}

static gboolean
get_block_number (const char *block_id, void *data)
{
    guint64 *n_blocks = data;

    ++(*n_blocks);

    return TRUE;
}

guint64
winguf_block_manager_get_block_number (SeafBlockManager *mgr)
{
    guint64 n_blocks = 0;

    winguf_block_manager_foreach_block (mgr, get_block_number, &n_blocks);

    return n_blocks;
}

