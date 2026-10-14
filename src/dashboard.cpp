#include "dashboard.h"
#include <windows.h>
#include <algorithm>
#include <string>
#include <sstream>

void PerfHistory::add(const PerfSample& s) {
    PerfSample safe = s;
    safe.cpu = std::clamp(safe.cpu, 0.0, 100.0);
    safe.ram_percent = std::clamp(safe.ram_percent, 0.0, 100.0);
    safe.gpu = std::clamp(safe.gpu, 0.0, 100.0);
    data_.push_back(safe);
    if (data_.size() > 60) data_.pop_front();
}
void PerfHistory::clear() { data_.clear(); }
const std::deque<PerfSample>& PerfHistory::samples() const { return data_; }

void draw_graph(HDC dc, const RECT& rc, const PerfHistory& history) {
    FillRect(dc, &rc, (HBRUSH)(COLOR_WINDOW+1));
    const auto& a = history.samples();
    SetBkMode(dc, TRANSPARENT);
    if (a.empty()) {
        const wchar_t* msg = L"Collecting CPU, RAM and GPU samples...";
        TextOutW(dc, rc.left + 12, rc.top + 12, msg, lstrlenW(msg));
        return;
    }
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    int header = 54, left = rc.left + 52, right = rc.right - 16;
    int top = rc.top + header, bottom = rc.bottom - 28;
    if (right <= left || bottom <= top) return;
    wchar_t status[256]{};
    const auto& last = a.back();
    if (last.gpu_valid)
        swprintf_s(status, L"CPU %.0f%%    RAM %.0f%% (%llu MB used)    GPU engine activity %.0f%%",
                   last.cpu, last.ram_percent, last.ram_mb, last.gpu);
    else
        swprintf_s(status, L"CPU %.0f%%    RAM %.0f%% (%llu MB used)    GPU activity unavailable",
                   last.cpu, last.ram_percent, last.ram_mb);
    TextOutW(dc, rc.left + 12, rc.top + 12, status, lstrlenW(status));
    const wchar_t* subtitle = L"Last 60 samples — RAM plotted as percent of physical memory";
    TextOutW(dc, rc.left + 12, rc.top + 31, subtitle, lstrlenW(subtitle));
    HPEN grid = CreatePen(PS_SOLID, 1, RGB(220,220,220));
    HGDIOBJ old = SelectObject(dc, grid);
    for (int i=0;i<=4;i++) {
        int y = top + (bottom-top)*i/4;
        MoveToEx(dc,left,y,nullptr); LineTo(dc,right,y);
        wchar_t label[16]{}; swprintf_s(label,L"%d%%",100-i*25);
        TextOutW(dc,rc.left+8,y-8,label,lstrlenW(label));
    }
    SelectObject(dc, old); DeleteObject(grid);
    HPEN pens[3] = {CreatePen(PS_SOLID,2,RGB(40,90,180)),
                    CreatePen(PS_SOLID,2,RGB(40,150,80)),
                    CreatePen(PS_SOLID,2,RGB(190,90,35))};
    for (int series=0;series<3;series++) {
        SelectObject(dc,pens[series]);
        size_t n=a.size();
        bool started=false;
        for(size_t i=0;i<n;i++) {
            if(series==2 && !a[i].gpu_valid) { started=false; continue; }
            double v = series==0 ? a[i].cpu :
                       series==1 ? a[i].ram_percent : a[i].gpu;
            double vmax=100.0;
            if(series==1) vmax=100.0;
            int x=left+(int)((right-left)*(double)i/(double)std::max<size_t>(1,n-1));
            int y=bottom-(int)((bottom-top)*std::clamp(v/vmax,0.0,1.0));
            if(!started) { MoveToEx(dc,x,y,nullptr); started=true; }
            else LineTo(dc,x,y);
        }
    }
    for(auto pen:pens) DeleteObject(pen);
    SetTextColor(dc,RGB(40,90,180)); TextOutW(dc,left,bottom+6,L"CPU",3);
    SetTextColor(dc,RGB(40,150,80)); TextOutW(dc,left+45,bottom+6,L"RAM",3);
    SetTextColor(dc,RGB(190,90,35)); TextOutW(dc,left+95,bottom+6,L"GPU",3);
}
