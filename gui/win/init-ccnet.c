/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "common.h"

#include <glib/gi18n.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <ccnet.h>

#include "stdafx.h"
#ifdef SEAF_LANG_CHINESE
    #include "resource.h"
#else
    #include "resource.en.h"
#endif

#include "trayicon.h"
#include <commdlg.h>
#include <ShlObj.h>
#include <shlwapi.h>

#include "utils.h"
#include "ccnet-init.h"

#include "rpc-wrapper.h"
#include "wingufile-applet.h"
#include "applet-log.h"

#define INIT_CCNET_SUCCESS  0
#define INIT_CCNET_FAILED   -1

#define IS_DLG_CONTROL(ID)   ((HWND)(lParam) == GetDlgItem(hDlg, (ID)))

void
msgbox (HWND hWnd, char *format, ...)
{
    va_list params;
    char buf[2048];
    wchar_t *wbuf;

    va_start(params, format);
    vsnprintf(buf, sizeof(buf), format, params);
    va_end(params);

    wbuf = wchar_from_utf8 (buf);
    MessageBoxW (hWnd, wbuf, L"Wingufile", MB_ICONINFORMATION | MB_OK);

    g_free (wbuf);
}

BOOL
msgbox_yes_or_no (HWND hWnd, char *format, ...)
{
    va_list params;
    char buf[2048];
    wchar_t *wbuf;
    int res;

    va_start(params, format);
    vsnprintf(buf, sizeof(buf), format, params);
    va_end(params);

    wbuf = wchar_from_utf8 (buf);
    res = MessageBoxW (hWnd, wbuf, L"Wingufile", MB_ICONQUESTION | MB_YESNO);

    g_free (wbuf);
    return (res == IDYES);
}

void
make_wnd_foreground (HWND hwndDlg)
{
    /* Center the window */

    // Get the owner window and dialog box rectangles.

    HWND hwndOwner;
    RECT rc, rcDlg, rcOwner;
    if ((hwndOwner = GetParent(hwndDlg)) == NULL) {
        hwndOwner = GetDesktopWindow();
    }

    if (hwndOwner == applet->hWnd) {
        hwndOwner = GetDesktopWindow();
    }

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hwndDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that right and bottom
    // values represent the width and height, and then offset the owner again
    // to discard space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
    OffsetRect(&rc, -rc.left, -rc.top);
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

    // The new position is the sum of half the remaining space and the owner's
    // original position.

    SetWindowPos(hwndDlg,
                 HWND_TOP,
                 rcOwner.left + (rc.right / 2),
                 rcOwner.top + (rc.bottom / 2),
                 0, 0,          // Ignores size arguments.
                 SWP_NOSIZE);


    ShowWindow (hwndDlg,  SW_SHOWNORMAL );
    SetWindowPos (hwndDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    SetWindowPos (hwndDlg, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    SetForegroundWindow (hwndDlg);
}

void
set_control_font (HWND hControl, char *font)
{
    HFONT hFont;

    hFont = CreateFont(12,
                       0,
                       0,
                       0,
                       FW_BOLD,
                       0,
                       0,
                       0,
                       GB2312_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE,
                       NULL);

    SendMessage (hControl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1,0));
    DeleteObject(hFont);
}

void set_dlg_icon(HWND hDlg, UINT icon_id)
{
    static HICON hIcon = NULL;
    if (!hIcon) {
        hIcon = LoadIcon (applet->hInstance,
                          MAKEINTRESOURCE(icon_id));
    }
    SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

extern char *wingufile_bin_dir;

#if 0
static void
InitComboxList(HWND hDlg)
{
    HWND hwndGroupsBox = GetDlgItem(hDlg, IDC_COMBOX_DISK);
    wchar_t drives[SEAF_PATH_MAX];
    wchar_t *p;
    ULARGE_INTEGER free_space;
    ULARGE_INTEGER largest_free_space;
    int i = 0;
    int largest_disk_index = 0;

    largest_free_space.QuadPart = 0;
    SendMessage (hwndGroupsBox, CB_RESETCONTENT, 0, 0);

    GetLogicalDriveStringsW (sizeof(drives), drives);
    for (p = drives; *p != L'\0'; p += wcslen(p) + 1) {
        /* Skip floppy disk, network drive, etc */
        if (GetDriveTypeW(p) != DRIVE_FIXED)
            continue;

        if (GetDiskFreeSpaceExW (p, &free_space, NULL, NULL)) {
            if (free_space.QuadPart > largest_free_space.QuadPart) {
                largest_free_space.QuadPart = free_space.QuadPart;
                largest_disk_index = i;
            }
        } else {
            free_space.QuadPart = 0;
            applet_warning ("failed to GetDiskFreeSpaceEx(), GLE=%lu\n",
                            GetLastError());
        }

        wchar_t wbuf[128];
        wchar_t *trans;

        if (free_space.QuadPart) {
            double space = ((double)(free_space.QuadPart)) / (1024 * 1024 * 1024);

            trans = wchar_from_utf8 (_("free"));
            _snwprintf (wbuf, sizeof(wbuf) / sizeof(wchar_t),
                       L"%s\t   (%.1f GB %s)",
                       p, space, trans);
        } else {
            trans = wchar_from_utf8 (_("free space unknown"));
            _snwprintf (wbuf, sizeof(wbuf), L"%s\t   (%s)", p, trans);
        }
        g_free (trans);

        i++;

        SendMessageW (hwndGroupsBox, CB_ADDSTRING, 0, (LPARAM) wbuf);
    }

    SendDlgItemMessage (hDlg, IDC_COMBOX_DISK, CB_SETCURSEL, largest_disk_index, 0);
}
#endif

static wchar_t *
get_largest_disk()
{
    wchar_t drives[SEAF_PATH_MAX];
    wchar_t *p, *largest_drive;
    ULARGE_INTEGER free_space;
    ULARGE_INTEGER largest_free_space;

    largest_free_space.QuadPart = 0;
    largest_drive = NULL;

    GetLogicalDriveStringsW (sizeof(drives), drives);
    for (p = drives; *p != L'\0'; p += wcslen(p) + 1) {
        /* Skip floppy disk, network drive, etc */
        if (GetDriveTypeW(p) != DRIVE_FIXED)
            continue;

        if (GetDiskFreeSpaceExW (p, &free_space, NULL, NULL)) {
            if (free_space.QuadPart > largest_free_space.QuadPart) {
                largest_free_space.QuadPart = free_space.QuadPart;
                if (largest_drive != NULL) {
                    free (largest_drive);
                }
                largest_drive = wcsdup(p);
            }

        } else {
            applet_warning ("failed to GetDiskFreeSpaceEx(), GLE=%lu\n",
                            GetLastError());
        }
    }

    return largest_drive;
}

/* 1. set wingufile-data directory to hidden
   2. set wingufile folder icon (via Desktop.ini)
*/
static void
set_wingufdir_attributes ()
{
    char *wingufdir = g_path_get_dirname (applet->wingufile_dir);
    char *icon_path = NULL;
    char *ini_file_path = NULL;

    wchar_t *wingufile_dir_w = NULL; /* C:\\Wingufile\\wingufile-data */
    wchar_t *wingufdir_w =  NULL;    /* C:\\Wingufile */
    wchar_t *icon_path_w = NULL; /* C:\\Program Files\\Wingufile\\bin\\wingufdir.ico */
    wchar_t * ini_file_path_w = NULL; /* C:\\Wingufile\\Desktop.ini */

    FILE *ini_file = NULL;

    wingufile_dir_w = wchar_from_utf8 (applet->wingufile_dir);
    wingufdir_w = wchar_from_utf8 (wingufdir);

    /* Make wingufile-data directory hidden. */
    SetFileAttributesW (wingufile_dir_w,
                        FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    /* Set wingufdir folder icon. */
    SetFileAttributesW (wingufdir_w, FILE_ATTRIBUTE_SYSTEM);
    ini_file_path = g_build_filename (wingufdir, "Desktop.ini", NULL);
    ini_file_path_w = wchar_from_utf8 (ini_file_path);

    if (!(ini_file = g_fopen(ini_file_path, "w"))) {
        applet_warning ("failed to open %s\n", ini_file_path);
        goto out;
    }

    icon_path = g_build_filename (wingufile_bin_dir, "wingufdir.ico", NULL);

    char *ptr = icon_path;
    while (*ptr != '\0') {
        /* Replace all / with \ */
        if (*ptr == '/')
            *ptr = '\\';

        ptr++;
    }
    icon_path_w = wchar_from_utf8 (icon_path);

    fwprintf (ini_file, L"[.ShellClassInfo]\n");
    fwprintf (ini_file, L"IconFile=%s\n", icon_path_w);
    fwprintf (ini_file, L"IconIndex=0\n");

    /* Make the "Desktop.ini" file hidden. */
    SetFileAttributesW (ini_file_path_w,
                        FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

out:
    g_free (wingufile_dir_w);
    g_free (wingufdir);
    g_free (wingufdir_w);
    g_free (ini_file_path);
    g_free (ini_file_path_w);
    g_free (icon_path);
    g_free (icon_path_w);

    if (ini_file) fclose (ini_file);
}

extern char *wingufile_bin_dir;

/* Copy manual from install dir to Wingufile directory */
void
copy_user_manual ()
{
    char *installdir;            /* C:\Program Files\Wingufile */
    char *wingufdir;              /* C:\Wingufile */
    char *src_path;             /* C:\Program Files\Wingufile\help.txt */
    char *dst_path;             /* C:\Wingufile\help.txt */

    wchar_t *src_path_w, *dst_path_w;

    installdir = g_path_get_dirname (wingufile_bin_dir);
    wingufdir = g_path_get_dirname (applet->wingufile_dir);

    src_path = g_build_filename (installdir, _("Wingufile help.txt"), NULL);
    dst_path = g_build_filename (wingufdir, _("Wingufile help.txt"), NULL);

    src_path_w = wchar_from_utf8 (src_path);
    dst_path_w = wchar_from_utf8 (dst_path);

    BOOL failIfExist = FALSE;
    CopyFileW (src_path_w, dst_path_w, failIfExist);

    g_free (installdir);
    g_free (wingufdir);
    g_free (src_path);
    g_free (dst_path);
    g_free (src_path_w);
    g_free (dst_path_w);
}

static wchar_t*
browse_for_folder()
{
    wchar_t path[MAX_PATH];
    wchar_t *title = wchar_from_utf8(_("Choose a folder"));
    BROWSEINFOW info = {0};
    wchar_t *ret = NULL;

    info.pidlRoot = NULL;       /* file system root */
    info.lpszTitle = title;     /* dialog title text */
    info.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE;
    info.lpfn = NULL;           /* event callback function  */

    ITEMIDLIST *idlist = SHBrowseForFolderW(&info);
    if (idlist != NULL) {
        SHGetPathFromIDListW(idlist, path);
        if (wcslen(path) != 0) {
            ret =  wcsdup(path);
        }
    }

    g_free (title);

    return ret;
}

static BOOL CALLBACK
InitWingufileProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int len;
    wchar_t wingufiledir[SEAF_PATH_MAX];

    memset(wingufiledir, 0, sizeof(wingufiledir));

    switch (message) {
    case WM_CREATE:
        break;

    case WM_INITDIALOG: {
        set_dlg_icon (hDlg, IDI_WINGUFILE_ICON);
        set_control_font (GetDlgItem(hDlg, IDC_STATIC_TITLE), "Courier");
        wchar_t* dft = get_largest_disk();
        if (dft) {
            SetWindowTextW (GetDlgItem(hDlg, IDC_EDIT_WINGUFILE_DIR), dft);
            free (dft);
        }
        make_wnd_foreground(hDlg);
        return TRUE;
    }

    case WM_CTLCOLORSTATIC:
        if (IS_DLG_CONTROL (IDC_STATIC_TITLE)) {

            SetBkMode((HDC)wParam, TRANSPARENT);
            return (BOOL)GetStockObject(NULL_BRUSH);
        }
        break;

    case WM_COMMAND:
        /* Return 0 if we handle it, 1 if not */
        if (IS_DLG_CONTROL(IDC_BUTTON_CHOOSE_WINGUFILE_DIR)) {
            WORD cmd = HIWORD(wParam);
            if (cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED) {
                wchar_t *path = browse_for_folder();
                if (path != NULL) {
                    SetWindowTextW (GetDlgItem(hDlg, IDC_EDIT_WINGUFILE_DIR), path);
                }
                free (path);
                return 0;
            }
        }
        switch(LOWORD(wParam)) {
        case IDOK: {
            len = GetWindowTextW (GetDlgItem(hDlg, IDC_EDIT_WINGUFILE_DIR),
                                  wingufiledir, G_N_ELEMENTS(wingufiledir));
            if (len == 0) {
                msgbox (applet->hWnd, _("Please choose a folder first"));
                return 0;
            }

            char *dir = wchar_to_utf8(wingufiledir);

            applet->wingufile_dir = g_build_filename(dir,
                                                   "Wingufile",
                                                   "wingufile-data", NULL);

            g_free (dir);

            if (checkdir_with_mkdir(applet->wingufile_dir) < 0) {
                g_free (applet->wingufile_dir);
                applet->wingufile_dir = NULL;
                EndDialog (hDlg, INIT_CCNET_FAILED);
                return TRUE;
            }

            set_wingufdir_attributes ();
            EndDialog (hDlg, INIT_CCNET_SUCCESS);
            return TRUE;
        }

        case IDCANCEL:
            if (msgbox_yes_or_no (hDlg, _("Initialzation is not finished. Really quit?"))) {
                EndDialog (hDlg, INIT_CCNET_FAILED);
            }
            return TRUE;
        default:
            break;
        }
        break;

    case WM_CLOSE:
        if (msgbox_yes_or_no (hDlg, _("Initialzation is not finished. Really quit?"))) {
            EndDialog (hDlg, INIT_CCNET_FAILED);
        }
        return TRUE;

    default:
        break;
    }
    return 0;
}

BOOL first_use = FALSE;

int
show_init_wingufile_window ()
{
    int response;

    first_use = TRUE;

    response = DialogBox (applet->hInstance,
                          MAKEINTRESOURCE(IDD_INIT_WINGUFILE),
                          applet->hWnd, InitWingufileProc);
    if (response != INIT_CCNET_SUCCESS) {
        return -1;
    }

    return 0;
}

static inline void
open_browser(const char *url)
{
    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
