#ifndef OBJ_STORE_H
#define OBJ_STORE_H

#include <glib.h>
#include <sys/types.h>

struct _WingufileSession;
struct SeafObjStore;
struct CEventManager;

struct SeafObjStore *
winguf_obj_store_new (struct _WingufileSession *winguf, const char *obj_type);

int
winguf_obj_store_init (struct SeafObjStore *obj_store,
                     gboolean enable_async,
                     struct CEventManager *ev_mgr);

/* Synchronous I/O interface. */

int
winguf_obj_store_read_obj (struct SeafObjStore *obj_store,
                         const char *obj_id,
                         void **data,
                         int *len);

int
winguf_obj_store_write_obj (struct SeafObjStore *obj_store,
                          const char *obj_id,
                          void *data,
                          int len);

gboolean
winguf_obj_store_obj_exists (struct SeafObjStore *obj_store,
                           const char *obj_id);

void
winguf_obj_store_delete_obj (struct SeafObjStore *obj_store,
                           const char *obj_id);

/* Asynchronous I/O interface. */

typedef struct OSAsyncResult {
    guint32 rw_id;
    char    obj_id[41];
    /* @data is owned by obj-store, don't free it. */
    void    *data;
    int     len;
    gboolean success;
} OSAsyncResult;

typedef void (*OSAsyncCallback) (OSAsyncResult *res, void *cb_data);

/* Async read */
guint32
winguf_obj_store_register_async_read (struct SeafObjStore *obj_store,
                                    OSAsyncCallback callback,
                                    void *cb_data);

void
winguf_obj_store_unregister_async_read (struct SeafObjStore *obj_store,
                                      guint32 reader_id);

int
winguf_obj_store_async_read (struct SeafObjStore *obj_store,
                           guint32 reader_id,
                           const char *obj_id);

/* Async write */
guint32
winguf_obj_store_register_async_write (struct SeafObjStore *obj_store,
                                     OSAsyncCallback callback,
                                     void *cb_data);

void
winguf_obj_store_unregister_async_write (struct SeafObjStore *obj_store,
                                       guint32 writer_id);

int
winguf_obj_store_async_write (struct SeafObjStore *obj_store,
                            guint32 writer_id,
                            const char *obj_id,
                            const void *obj_data,
                            int data_len);

/* Async stat */
guint32
winguf_obj_store_register_async_stat (struct SeafObjStore *obj_store,
                                    OSAsyncCallback callback,
                                    void *cb_data);

void
winguf_obj_store_unregister_async_stat (struct SeafObjStore *obj_store,
                                      guint32 stat_id);

int
winguf_obj_store_async_stat (struct SeafObjStore *obj_store,
                           guint32 stat_id,
                           const char *obj_id);

#endif
