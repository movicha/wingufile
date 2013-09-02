#include <windows.h>

#define S_WINDOW_NAME "wingufile-applet"

/* UINT __stdcall TerminateWingufile(MSIHANDLE hModule) */
UINT __stdcall TerminateWingufile(HANDLE hModule)
{
    HWND hWnd = FindWindow(S_WINDOW_NAME, S_WINDOW_NAME);
    if (hWnd)
    {
        PostMessage(hWnd, WM_CLOSE, (WPARAM)NULL, (LPARAM)NULL);
        int i;
        for (i = 0; i < 10; ++i)
        {
            Sleep(500);
            if (!IsWindow(hWnd))
            {
                /* wingufile-applet is now killed. */
                return ERROR_SUCCESS;
            }
        }
        return ERROR_SUCCESS;
    }
    
    /* wingufile-applet is not running. */
    return ERROR_SUCCESS;
}

/* Remove auto start entry for wingufile when uninstall. Error is ignored. */
UINT __stdcall RemoveWingufileAutoStart(HANDLE hModule)
{
    const char *key_run = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char *appname = "Wingufile";
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
                               key_run,
                               0L,KEY_WRITE | KEY_READ,
                               &hKey);
    if (result != ERROR_SUCCESS) {
        goto out;
    }

    result = RegDeleteValue (hKey, appname);
    RegCloseKey(hKey);

out:
    return ERROR_SUCCESS;
}
