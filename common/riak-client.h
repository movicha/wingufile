#ifndef SEAF_RIAK_CLIENT_H
#define SEAF_RIAK_CLIENT_H

#include <glib.h>

#define RIAK_QUORUM -1
#define RIAK_ALL -2

struct SeafRiakClient;
typedef struct SeafRiakClient SeafRiakClient;

SeafRiakClient *
winguf_riak_client_new (const char *host, const char *port);

void
winguf_riak_client_free (SeafRiakClient *client);

int
winguf_riak_client_get (SeafRiakClient *client,
                      const char *bucket,
                      const char *key,
                      void **value,
                      int *size);

int
winguf_riak_client_put (SeafRiakClient *client,
                      const char *bucket,
                      const char *key,
                      void *value,
                      int size,
                      int n_w);

gboolean
winguf_riak_client_query (SeafRiakClient *client,
                        const char *bucket,
                        const char *key);

int
winguf_riak_client_delete (SeafRiakClient *client,
                         const char *bucket,
                         const char *key,
                         int n_w);

#endif
