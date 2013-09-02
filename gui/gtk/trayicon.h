/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef TRAYICON_H
#define TRAYICON_H

#include <glib.h>

#define WINGUFILE_TYPE_TRAY_ICON         (wingufile_tray_icon_get_type ())
#define WINGUFILE_TRAY_ICON(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), WINGUFILE_TYPE_TRAY_ICON, WingufileTrayIcon))
#define WINGUFILE_TRAY_ICON_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), WINGUFILE_TYPE_TRAY_ICON, WingufileTrayIconClass))
#define WINGUFILE_IS_TRAY_ICON(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), WINGUFILE_TYPE_TRAY_ICON))
#define WINGUFILE_IS_TRAY_ICON_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), WINGUFILE_TYPE_TRAY_ICON))
#define WINGUFILE_TRAY_ICON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), WINGUFILE_TYPE_TRAY_ICON, WingufileTrayIconClass))

typedef struct _WingufileTrayIcon      WingufileTrayIcon;
typedef struct _WingufileTrayIconClass WingufileTrayIconClass;

typedef struct WingufileTrayIconPriv WingufileTrayIconPriv;

struct _WingufileTrayIcon {
        GObject parent;
        WingufileTrayIconPriv *priv;
};

struct _WingufileTrayIconClass {
        GObjectClass parent_class;
};

GType wingufile_trayicon_get_type (void);

WingufileTrayIcon *wingufile_trayicon_new ();

#define ICON_STATUS_UP	    "ccnet_daemon_up"
#define ICON_STATUS_DOWN    "ccnet_daemon_down"
#define ICON_AUTO_SYNC_DISABLED    "wingufile_auto_sync_disabled"

#define WINGUFILE_TRANFER_1   "wingufile_transfer_1"
#define WINGUFILE_TRANFER_2   "wingufile_transfer_2"
#define WINGUFILE_TRANFER_3   "wingufile_transfer_3"
#define WINGUFILE_TRANFER_4   "wingufile_transfer_4"

void wingufile_trayicon_set_icon (WingufileTrayIcon *icon, const char *name);

void wingufile_trayicon_notify (WingufileTrayIcon *icon, char *title, char *buf);

void wingufile_trayicon_set_tooltip (WingufileTrayIcon *icon, const char *tooltip);

void reset_trayicon_and_tip (WingufileTrayIcon *icon);

#endif /* __WINGUFILE_TRAY_ICON_H__ */
