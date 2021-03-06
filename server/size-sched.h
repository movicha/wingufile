#ifndef SIZE_SCHEDULER_H
#define SIZE_SCHEDULER_H

struct _WingufileSession;

struct SizeSchedulerPriv;

typedef struct SizeScheduler {
    struct _WingufileSession *winguf;

    struct SizeSchedulerPriv *priv;
} SizeScheduler;

SizeScheduler *
size_scheduler_new (struct _WingufileSession *session);

int
size_scheduler_start (SizeScheduler *scheduler);

void
schedule_repo_size_computation (SizeScheduler *scheduler, const char *repo_id);

#endif
