/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef SEAF_FILE_MGR_H
#define SEAF_FILE_MGR_H

#include <glib.h>
#include "bitfield.h"

#include "wingufile-object.h"

#include "obj-store.h"

#include "cdc/cdc.h"
#include "../common/wingufile-crypt.h"

typedef struct _SeafFSManager SeafFSManager;
typedef struct _Seafile Seafile;
typedef struct _SeafDir SeafDir;
typedef struct _SeafDirent SeafDirent;

struct _Seafile {
    char        file_id[41];
    guint64     file_size;
    guint32     n_blocks;
    char        **blk_sha1s;
    int         ref_count;
};

void
wingufile_ref (Seafile *wingufile);

void
wingufile_unref (Seafile *wingufile);

typedef enum {
    SEAF_METADATA_TYPE_INVALID,
    SEAF_METADATA_TYPE_FILE,
    SEAF_METADATA_TYPE_LINK,
    SEAF_METADATA_TYPE_DIR,
} SeafMetadataType;

#define SEAF_DIR_NAME_LEN 256


struct _SeafDirent {
    guint32    mode;
    char       id[41];
    guint32    name_len;
    char       name[SEAF_DIR_NAME_LEN];
};

struct _SeafDir {
    char   dir_id[41];
    GList *entries;
};

SeafDir *
winguf_dir_new (const char *id, GList *entries, gint64 ctime);

void 
winguf_dir_free (SeafDir *dir);

SeafDir *
winguf_dir_from_data (const char *dir_id, const uint8_t *data, int len);

int 
winguf_dir_save (SeafFSManager *fs_mgr, SeafDir *dir);

int
winguf_metadata_type_from_data (const uint8_t *data, int len);

SeafDirent *
winguf_dirent_new (const char *sha1, int mode, const char *name);

SeafDirent *
winguf_dirent_dup (SeafDirent *dent);

typedef struct {
    /* TODO: GHashTable may be inefficient when we have large number of IDs. */
    GHashTable  *block_hash;
    GPtrArray   *block_ids;
    Bitfield     block_map;
    uint32_t     n_blocks;
    uint32_t     n_valid_blocks;
} BlockList;

BlockList *
block_list_new ();

void
block_list_free (BlockList *bl);

void
block_list_generate_bitmap (BlockList *bl);

void
block_list_serialize (BlockList *bl, uint8_t **buffer, uint32_t *len);

void
block_list_insert (BlockList *bl, const char *block_id);

/* Return a blocklist containing block ids which are in @bl1 but
 * not in @bl2.
 */
BlockList *
block_list_difference (BlockList *bl1, BlockList *bl2);

struct _SeafileSession;

typedef struct _SeafFSManagerPriv SeafFSManagerPriv;

struct _SeafFSManager {
    struct _SeafileSession *winguf;

    struct SeafObjStore *obj_store;

    SeafFSManagerPriv *priv;
};

SeafFSManager *
winguf_fs_manager_new (struct _SeafileSession *winguf,
                     const char *winguf_dir);

int
winguf_fs_manager_init (SeafFSManager *mgr);

#ifndef WINGUFILE_SERVER

char *
winguf_fs_manager_checkin (SeafFSManager *mgr,
                         const char *path);

int 
winguf_fs_manager_checkout (SeafFSManager *mgr,
                          const char *root_id,
                          const char *output_path);

int 
winguf_fs_manager_checkout_file (SeafFSManager *mgr, 
                               const char *file_id, 
                               const char *file_path,
                               guint32 mode,
                               struct SeafileCrypt *crypt,
                               const char *conflict_suffix,
                               gboolean force_conflict,
                               gboolean *conflicted);

#endif  /* not WINGUFILE_SERVER */

/**
 * Check in blocks and create wingufile/symlink object.
 * Returns sha1 id for the wingufile/symlink object in @sha1 parameter.
 */
int
winguf_fs_manager_index_blocks (SeafFSManager *mgr,
                              const char *file_path,
                              unsigned char sha1[],
                              SeafileCrypt *crypt);

uint32_t
winguf_fs_manager_get_type (SeafFSManager *mgr, const char *id);

Seafile *
winguf_fs_manager_get_wingufile (SeafFSManager *mgr, const char *file_id);

SeafDir *
winguf_fs_manager_get_wingufdir (SeafFSManager *mgr, const char *dir_id);

/* Make sure entries in the returned dir is sorted in descending order.
 */
SeafDir *
winguf_fs_manager_get_wingufdir_sorted (SeafFSManager *mgr, const char *dir_id);

int
winguf_fs_manager_populate_blocklist (SeafFSManager *mgr,
                                    const char *root_id,
                                    BlockList *bl);

/*
 * For dir object, set *stop to TRUE to stop traversing the subtree.
 */
typedef gboolean (*TraverseFSTreeCallback) (SeafFSManager *mgr,
                                            const char *obj_id,
                                            int type,
                                            void *user_data,
                                            gboolean *stop);

int
winguf_fs_manager_traverse_tree (SeafFSManager *mgr,
                               const char *root_id,
                               TraverseFSTreeCallback callback,
                               void *user_data,
                               gboolean skip_errors);

gboolean
winguf_fs_manager_object_exists (SeafFSManager *mgr, const char *id);

gint64
winguf_fs_manager_get_file_size (SeafFSManager *mgr, const char *file_id);

gint64
winguf_fs_manager_get_fs_size (SeafFSManager *mgr, const char *root_id);

#ifndef WINGUFILE_SERVER
int
wingufile_write_chunk (CDCDescriptor *chunk,
                     SeafileCrypt *crypt,
                     uint8_t *checksum,
                     gboolean write_data);
#endif /* WINGUFILE_SERVER */

uint32_t
calculate_chunk_size (uint64_t total_size);

int
winguf_fs_manager_count_fs_files (SeafFSManager *mgr, const char *root_id);

SeafDir *
winguf_fs_manager_get_wingufdir_by_path(SeafFSManager *mgr,
                                    const char *root_id,
                                    const char *path,
                                    GError **error);
char *
winguf_fs_manager_get_wingufile_id_by_path (SeafFSManager *mgr,
                                        const char *root_id,
                                        const char *path,
                                        GError **error);

char *
winguf_fs_manager_path_to_obj_id (SeafFSManager *mgr,
                                 const char *root_id,
                                 const char *path,
                                 guint32 *mode,
                                 GError **error);

char *
winguf_fs_manager_get_wingufdir_id_by_path (SeafFSManager *mgr,
                                        const char *root_id,
                                        const char *path,
                                        GError **error);

#endif
