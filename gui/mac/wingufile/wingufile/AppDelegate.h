//
//  AppDelegate.h
//  wingufile
//
//  Created by Wei Wang on 5/15/12.
//  Copyright (c) 2012 tsinghua. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    IBOutlet NSMenu *statusMenu;

@public
    CFRunLoopRef loop;
    NSImage *statusImage[3];
    NSImage *transferImage[4];
    NSImage *warningImage;
    NSImage *repoImage;
}

@property (retain) NSTask *task;
@property CFRunLoopSourceRef sock;
@property (retain) NSStatusItem *statusItem;

@property (assign) IBOutlet NSTextFieldCell *bubbleTitle;
@property (assign) IBOutlet NSView *bubbleView;
@property (assign) IBOutlet NSTextField *bubbleText;

@property (assign) IBOutlet NSWindow *initWingufileWindow;
@property (assign) IBOutlet NSTextField *chooseDirField;
@property (assign) IBOutlet NSImageView *chooseDirWarnImg;

@property (assign) IBOutlet NSMenuItem *openBrowerItem;
@property (assign) IBOutlet NSMenuItem *restartItem;
@property (assign) IBOutlet NSMenuItem *enableAutoSyncItem;
@property (assign) IBOutlet NSMenuItem *disableAutoSyncItem;



- (IBAction)open_browser: (id)sender;
- (IBAction)restart: (id)sender;
- (IBAction)quit: (id)sender;
- (IBAction)openWingufileSite: (id)sender;
- (IBAction)enableAutoSync: (id)sender;
- (IBAction)disableAutoSync: (id)sender;


- (IBAction)initwingufile_ok: (id)sender;
- (IBAction)initwingufile_cancel: (id)sender;
- (IBAction)initwingufile_choose_dir: (id)sender;

- (void)popup_bubble: (const char *)title message: (const char *)msg;

- (int)show_initwingufile_window;

- (void)add_dir_to_sidebar: (NSString *)appPath;
- (void)add_conn_server_timer: (int)timeout_ms;
- (void)add_open_browser_timer: (int)timeout_ms;
- (void)del_open_browser_timer;
- (void)add_trayicon_rotate_timer: (int)timeout_ms;
- (void)add_heartbeat_monitor_timer: (int)timeout_ms;
- (void)add_app_as_login_item: (NSString *)appPath;
- (void)del_app_from_login_item: (NSString *)appPath;

@end
