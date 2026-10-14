#pragma once
#include <windows.h>
#include <deque>
#include <string>

struct PerfSample {
    double cpu;
    unsigned long long ram_mb;
    double ram_percent;
    unsigned long threads;
    double gpu;
    bool gpu_valid;
};

class PerfHistory {
public:
    void add(const PerfSample& s);
    void clear();
    const std::deque<PerfSample>& samples() const;
private:
    std::deque<PerfSample> data_;
};

void draw_graph(HDC dc, const RECT& rc, const PerfHistory& history);
void show_performance_dashboard(HINSTANCE inst, HWND owner, PerfHistory* history);
