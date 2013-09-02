/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 * Wingufile-controller is responsible for:
 *
 *    1. Start: start server processes:
 *
 *       - ccnet-server
 *       - winguf-server
 *       - winguf-mon
 *       - httpserver
 *
 *    2. Repair:
 *
 *       - ensure ccnet process availability by watching client->connfd
 *       - ensure server processes availablity by checking process is running periodically
 *         If some process has stopped working, try to restart it.
 *
 */

#ifndef WINGUFILE_CONTROLLER_H
#define WINGUFILE_CONTROLLER_H

typedef struct _WingufileController WingufileController;

enum {
    PID_CCNET = 0,
    PID_SERVER,
    PID_HTTPSERVER,
    N_PID
};

struct _WingufileController {
    char *config_dir;
    char *wingufile_dir;
    char *logdir;

    CcnetClient         *client;
    CcnetClient         *sync_client;
    CcnetMqclientProc   *mqclient_proc;

    guint               check_process_timer;
    guint               client_io_id;
    /* Decide whether to start winguf-server in cloud mode  */
    gboolean            cloud_mode;

    int                 pid[N_PID];
    char                *pidfile[N_PID];
};
#endif
