/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 * Mq-manager is responsible for: 
 * 
 *  - Publishing heartbeat messages every HEARTBEAT_INTERVAL senconds to
 *    indicate it's alive. If wingufile-applet doesn't get the message, it would
 *    check and try to restart winguf-daemon.
 *
 *  - Provide API for other modules to publish their messages.
 *
 * Currently we publish these types of messages:
 *
 *  - wingufile.heartbeat <>
 *  - wingufile.transfer <start | stop >
 *  - wingufile.repo_sync_done <repo-name>
 *  - wingufile.promt_create_repo <worktree>
 *  - wingufile.repo_created <repo-name>
 *
 * And subscribe to no messages. 
 */

#ifndef SEAF_MQ_MANAGER_H
#define SEAF_MQ_MANAGER_H

struct _CcnetMessage;

typedef struct _SeafMqManager SeafMqManager;

struct _SeafMqManager {
    struct _SeafileSession   *winguf;
    struct _SeafMqManagerPriv *priv;
};

SeafMqManager *winguf_mq_manager_new (struct _SeafileSession *winguf);   

void winguf_mq_manager_set_heartbeat_name (SeafMqManager *mgr, const char *app);

int winguf_mq_manager_init (SeafMqManager *mgr);

int winguf_mq_manager_start (SeafMqManager *mgr);


void winguf_mq_manager_publish_message (SeafMqManager *mgr,
                                      struct _CcnetMessage *msg);

void
winguf_mq_manager_publish_message_full (SeafMqManager *mgr,
                                      const char *app,
                                      const char *body,
                                      int flags);

void
winguf_mq_manager_publish_notification (SeafMqManager *mgr,
                                      const char *type,
                                      const char *content);

void
winguf_mq_manager_publish_event (SeafMqManager *mgr, const char *content);

#endif
