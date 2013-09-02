#ifndef CHUNKSERV_MGR_H
#define CHUNKSERV_MGR_H

#include <glib.h>
#include <db.h>

struct _SeafileSession;

struct _SeafCSManager {
    struct _SeafileSession      *winguf;
    GHashTable          *chunk_servers;
    sqlite3             *db;
};
typedef struct _SeafCSManager SeafCSManager;

SeafCSManager*  winguf_cs_manager_new (struct _SeafileSession *winguf);
int             winguf_cs_manager_start (SeafCSManager *mgr);

int             winguf_cs_manager_add_chunk_server (SeafCSManager *mgr, const char *cs_id);
int             winguf_cs_manager_del_chunk_server (SeafCSManager *mgr, const char *cs_id);
GList*          winguf_cs_manager_get_chunk_servers (SeafCSManager *mgr);

#endif
