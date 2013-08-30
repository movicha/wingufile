/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef TRAYICON_H
#define TRAYICON_H

#include <glib.h>

#define WINGUFILE_TYPE_TRAY_ICON         (wingufile_tray_icon_get_type ())
#define WINGUFILE_TRAY_ICON(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), WINGUFILE_TYPE_TRAY_ICON, SeafileTrayIcon))
#define WINGUFILE_TRAY_ICON_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), WINGUFILE_TYPE_TRAY_ICON, SeafileTrayIconClass))
#define WINGUFILE_IS_TRAY_ICON(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), WINGUFILE_TYPE_TRAY_ICON))
#define WINGUFILE_IS_TRAY_ICON_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), WINGUFILE_TYPE_TRAY_ICON))
#define WINGUFILE_TRAY_ICON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), WINGUFILE_TYPE_TRAY_ICON, SeafileTrayIconClass))

typedef struct _SeafileTrayIcon      SeafileTrayIcon;
typedef struct _SeafileTrayIconClass SeafileTrayIconClass;

typedef struct SeafileTrayIconPriv SeafileTrayIconPriv;

struct _SeafileTrayIcon {
        GObject parent;
        SeafileTrayIconPriv *priv;
};

struct _SeafileTrayIconClass {
        GObjectClass parent_class;
};

GType wingufile_trayicon_get_type (void);

SeafileTrayIcon *wingufile_trayicon_new ();

#define ICON_STATUS_UP	    "ccnet_daemon_up"
#define ICON_STATUS_DOWN    "ccnet_daemon_down"
#define ICON_AUTO_SYNC_DISABLED    "wingufile_auto_sync_disabled"

#define WINGUFILE_TRANFER_1   "wingufile_transfer_1"
#define WINGUFILE_TRANFER_2   "wingufile_transfer_2"
#define WINGUFILE_TRANFER_3   "wingufile_transfer_3"
#define WINGUFILE_TRANFER_4   "wingufile_transfer_4"

void wingufile_trayicon_set_icon (SeafileTrayIcon *icon, const char *name);

void wingufile_trayicon_notify (SeafileTrayIcon *icon, char *title, char *buf);

void wingufile_trayicon_set_tooltip (SeafileTrayIcon *icon, const char *tooltip);

void reset_trayicon_and_tip (SeafileTrayIcon *icon);

#endif /* __WINGUFILE_TRAY_ICON_H__ */
