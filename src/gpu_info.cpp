#include "gpu_info.h"
#include <windows.h>
#include <dxgi.h>
#include <wrl/client.h>

#pragma comment(lib, "dxgi.lib")

std::vector<GpuInfo> enumerate_gpus() {
    std::vector<GpuInfo> out;
    Microsoft::WRL::ComPtr<IDXGIFactory1> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return out;

    for (UINT i=0;;++i) {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        if (factory->EnumAdapters1(i,&adapter)==DXGI_ERROR_NOT_FOUND) break;
        DXGI_ADAPTER_DESC1 d{};
        if (FAILED(adapter->GetDesc1(&d))) continue;
        if (d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        GpuInfo g;
        g.name = d.Description;
        g.dedicated_mb = d.DedicatedVideoMemory / (1024ull*1024ull);
        g.shared_mb = d.SharedSystemMemory / (1024ull*1024ull);
        out.push_back(g);
    }
    return out;
}
