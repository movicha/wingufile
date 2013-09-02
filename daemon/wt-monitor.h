#ifndef SEAF_WT_MONITOR_H
#define SEAF_WT_MONITOR_H

typedef struct WTStatus {
    char        repo_id[37];
    gint        last_check;
    gint        last_changed;
} WTStatus;

typedef struct SeafWTMonitorPriv SeafWTMonitorPriv;

struct _WingufileSession;

typedef struct SeafWTMonitor {
    struct _WingufileSession      *winguf;
    SeafWTMonitorPriv   *priv;
} SeafWTMonitor;

SeafWTMonitor *
winguf_wt_monitor_new (struct _WingufileSession *winguf);

int
winguf_wt_monitor_start (SeafWTMonitor *monitor);

int
winguf_wt_monitor_watch_repo (SeafWTMonitor *monitor, const char *repo_id);

int
winguf_wt_monitor_unwatch_repo (SeafWTMonitor *monitor, const char *repo_id);

int
winguf_wt_monitor_refresh_repo (SeafWTMonitor *monitor, const char *repo_id);

WTStatus *
winguf_wt_monitor_get_worktree_status (SeafWTMonitor *monitor,
                                     const char *repo_id);

#endif
