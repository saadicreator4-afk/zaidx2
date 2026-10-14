#pragma once
#include <string>
#include <vector>

struct GpuInfo {
    std::wstring name;
    unsigned long long dedicated_mb = 0;
    unsigned long long shared_mb = 0;
};

std::vector<GpuInfo> enumerate_gpus();
