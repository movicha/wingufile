//
//  platform.h
//  wingufile
//
//  Created by Wei Wang on 6/9/12.
//  Copyright (c) 2012 tsinghua. All rights reserved.
//

#ifndef wingufile_platform_h
#define wingufile_platform_h

#define WINGUFILEWEBBUNDLE @"wingufile.wingufileweb"
#define NS_SEAF_HTTP_ADDR @"127.0.0.1:13420"


void show_warning (const char *title, const char *fmt, ...);

void shutdown_process (const char *name);

int is_process_already_running (const char *name);


int msgbox_yes_or_no (char *format, ...);

gboolean trayicon_do_rotate (void);

void wingufile_set_wingufilefolder_icns (void);

void wingufile_set_repofolder_icns (const char *path);

void wingufile_unset_repofolder_icns (const char *path);

int set_visibility_for_file (const char *filename, int isDirectory, int visible);

void reset_trayicon_and_tip (void);

#endif
