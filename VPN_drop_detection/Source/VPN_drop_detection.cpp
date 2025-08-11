// VPN_drop_detection.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include "VPN_drop_detection.h"
#include <shellapi.h>
#include <wininet.h>
#include <string>
#include "../Resource/Resource.h"

NOTIFYICONDATA nid = {};
HINSTANCE hInst;
UINT_PTR timerId;
std::string lastIP = "unknown";
LPCSTR CLASS_NAME = "VPNDropDetection";

std::string GetPublicIP() 
{
    HINTERNET hInternet = InternetOpen(CLASS_NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) 
        return "error";
    
    HINTERNET hConnect = InternetOpenUrl(hInternet, "https://api.ipify.org", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    
    if (!hConnect) { InternetCloseHandle(hInternet);
        return "error"; }

    char buffer[64] = {};
    DWORD bytesRead;
    InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return std::string(buffer);
}

void ShowNotification(const std::string& msg) {
    nid.uFlags = NIF_INFO;
    strcpy_s(nid.szInfoTitle, CLASS_NAME);
    strcpy_s(nid.szInfo, msg.c_str());
    nid.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void CheckIP(HWND hwnd) {
    
    std::string ip = GetPublicIP();
    if (ip != "error" && ip != lastIP) 
    {
        lastIP = ip;
        std::string wip(ip.begin(), ip.end());
        ShowNotification("IP changed to: " + wip);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
    switch (msg)
    {
        case WM_CREATE:
            // Tray icon
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = 1; // Just a unique ID
            nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            nid.uCallbackMessage = WM_USER + 1;

            nid.hIcon = (HICON)LoadImage(
                hInst,
                MAKEINTRESOURCE(IDI_APP_ICON),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                0
            );

            if (!nid.hIcon) {
                char buf[256];
                wsprintf(buf, "Failed to load icon. Error: %lu", GetLastError());
                MessageBox(NULL, buf, "Error", MB_OK);
            }

            strcpy_s(nid.szTip, CLASS_NAME);

            if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
                MessageBox(NULL, "Systray Icon Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
                return 0;
            }

            timerId = SetTimer(hwnd, 1, 30000, NULL);
            return 0;

        case WM_TIMER:
            CheckIP(hwnd);
            return 0;

        case WM_USER + 1:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAYMENU));
                HMENU hSubMenu = GetSubMenu(hMenu, 0);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hSubMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
            }
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDM_EXIT) {
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
            return 0;

        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            KillTimer(hwnd, timerId);
            PostQuitMessage(0);
            return 0;

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nShowCmd)
{
    hInst = hInstance;

    WNDCLASS wc = { };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        CLASS_NAME,                     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_HIDE);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;
}