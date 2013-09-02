#include <stdio.h>

#include "riak-client.h"

int main (int argc, char **argv)
{
    SeafRiakClient *client = winguf_riak_client_new ("127.0.0.1", "8098");
    int ret;
    char *value;
    int size;

    if (!winguf_riak_client_query (client, "test", "http-test")) {
        printf ("Writing value.\n");
        ret = winguf_riak_client_put (client, "test", "http-test",
                                    "test1", 6, 1);
        if (ret < 0) {
            g_error ("Failed to write.\n");
        }
    }

    ret = winguf_riak_client_get (client, "test", "http-test", (void**)&value, &size);
    if (ret < 0) {
        g_error ("Failed to read.\n");
    }

    printf ("Read value is %s.\n\n", value);

    ret = winguf_riak_client_delete (client, "test", "http-test", 1);
    if (ret < 0) {
        g_error ("Failed to delete.\n");
    }

    return 0;
}
