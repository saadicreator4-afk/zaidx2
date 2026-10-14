
#include "dashboard.h"
#include <windows.h>

static const wchar_t* kDashClass = L"ZAIDX_PERF_DASHBOARD";
static HWND gDashboardWindow = nullptr;

LRESULT CALLBACK DashboardWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    auto* history = reinterpret_cast<PerfHistory*>(GetWindowLongPtrW(h, GWLP_USERDATA));
    if (m == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(l);
        history = reinterpret_cast<PerfHistory*>(cs->lpCreateParams);
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)history);
    }
    if (m == WM_CREATE) {
        gDashboardWindow = h;
        SetTimer(h, 1, 1000, nullptr);
        return 0;
    }
    if (m == WM_TIMER) { InvalidateRect(h, nullptr, FALSE); return 0; }
    if (m == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC dc=BeginPaint(h,&ps);
        RECT r{}; GetClientRect(h,&r);
        if (history) {
            draw_graph(dc,r,*history);
        } else {
            const wchar_t* message = L"Performance history is unavailable.";
            TextOutW(dc, 12, 12, message, lstrlenW(message));
        }
        EndPaint(h,&ps);
        return 0;
    }
    if (m == WM_ERASEBKGND) return 1;
    if (m == WM_DESTROY) {
        KillTimer(h, 1);
        if (gDashboardWindow == h) gDashboardWindow = nullptr;
    }
    return DefWindowProcW(h,m,w,l);
}

void show_performance_dashboard(HINSTANCE inst, HWND owner, PerfHistory* history) {
    if (!history) return;
    if (gDashboardWindow && IsWindow(gDashboardWindow)) {
        ShowWindow(gDashboardWindow, SW_RESTORE);
        SetForegroundWindow(gDashboardWindow);
        InvalidateRect(gDashboardWindow, nullptr, TRUE);
        return;
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc=DashboardWndProc;
    wc.hInstance=inst;
    wc.lpszClassName=kDashClass;
    wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    RegisterClassW(&wc);

    HWND w=CreateWindowExW(0,kDashClass,L"ZAIDX Performance Dashboard",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT,CW_USEDEFAULT,720,420,
        owner,nullptr,inst,history);
    if(w) UpdateWindow(w);
}
