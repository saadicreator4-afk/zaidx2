#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <sstream>
#include <string>
#include <cwchar>
#include <cwctype>
#include <algorithm>
#include "system_info.h"

namespace zaidx {
static std::wstring vendor_from_id(UINT id) {
    switch (id) {
        case 0x10DE: return L"NVIDIA";
        case 0x1002: return L"AMD";
        case 0x8086: return L"Intel";
        default: return L"Unknown";
    }
}
static unsigned long long filetime_to_u64(const FILETIME& f) {
    ULARGE_INTEGER u{}; u.LowPart=f.dwLowDateTime; u.HighPart=f.dwHighDateTime; return u.QuadPart;
}
static double sample_cpu() {
    static ULONGLONG prevIdle=0, prevKernel=0, prevUser=0;
    FILETIME idle{}, kernel{}, user{};
    if (!GetSystemTimes(&idle,&kernel,&user)) return 0.0;
    auto i=filetime_to_u64(idle), k=filetime_to_u64(kernel), u=filetime_to_u64(user);
    if (!prevKernel) { prevIdle=i; prevKernel=k; prevUser=u; return 0.0; }
    // FILETIME counters should only increase; reset the baseline if the
    // counters roll back (for example after unusual system/VM transitions).
    if (i < prevIdle || k < prevKernel || u < prevUser) {
        prevIdle=i; prevKernel=k; prevUser=u;
        return 0.0;
    }
    auto di=i-prevIdle, dk=k-prevKernel, du=u-prevUser;
    prevIdle=i; prevKernel=k; prevUser=u;
    auto total=dk+du;
    if (!total || di > total) return 0.0;
    return std::clamp(100.0 * (double)(total-di)/(double)total, 0.0, 100.0);
}
SystemInfo query_system_info() {
    SystemInfo s{};
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    s.logical_cpus = si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1;
    MEMORYSTATUSEX ms{sizeof(ms)};
    if (GlobalMemoryStatusEx(&ms)) { s.total_ram_mb=ms.ullTotalPhys/1048576ull; s.available_ram_mb=ms.ullAvailPhys/1048576ull; }
    s.cpu_load_percent=sample_cpu();
    IDXGIFactory1* f=nullptr;
    if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&f))) {
        IDXGIAdapter1* a=nullptr;
        for(UINT i=0; f->EnumAdapters1(i,&a)!=DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 d{};
            if(SUCCEEDED(a->GetDesc1(&d)) && !(d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && s.gpu_name.empty()) {
                s.gpu_name=d.Description; s.gpu_vendor=vendor_from_id(d.VendorId);
            }
            a->Release(); a=nullptr;
        }
        f->Release();
    }
    return s;
}
std::wstring get_gpu_list_text() {
    std::wstringstream out; IDXGIFactory1* f=nullptr;
    if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&f))) return L"GPU enumeration unavailable.\r\n";
    IDXGIAdapter1* a=nullptr;
    for(UINT i=0; f->EnumAdapters1(i,&a)!=DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 d{};
        if(SUCCEEDED(a->GetDesc1(&d))) out<<L"GPU "<<i<<L": "<<d.Description<<L" | "<<vendor_from_id(d.VendorId)<<L" | VRAM "<<d.DedicatedVideoMemory/1048576ull<<L" MB\r\n";
        a->Release(); a=nullptr;
    }
    f->Release(); return out.str();
}
std::wstring get_process_list_text() {
    std::wstringstream out; HANDLE s=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(s==INVALID_HANDLE_VALUE) return L"Unable to enumerate processes.\r\n";
    PROCESSENTRY32W e{sizeof(e)};
    if(Process32FirstW(s,&e)) do { out<<e.th32ProcessID<<L"  "<<e.szExeFile<<L"\r\n"; } while(Process32NextW(s,&e));
    CloseHandle(s); return out.str();
}
std::wstring get_process_list_items() {
    std::wstringstream out; HANDLE s=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(s==INVALID_HANDLE_VALUE) return L"";
    PROCESSENTRY32W e{sizeof(e)};
    if(Process32FirstW(s,&e)) do { out<<e.th32ProcessID<<L"|" << e.szExeFile << L"\r\n"; } while(Process32NextW(s,&e));
    CloseHandle(s); return out.str();
}
bool get_process_info(unsigned long pid, std::wstring& name, unsigned long long& working_set_mb) {
    HANDLE h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_VM_READ,FALSE,pid);
    if(!h) return false;
    wchar_t path[1024]{}; DWORD n=1024;
    if(QueryFullProcessImageNameW(h,0,path,&n)) {
        const wchar_t* slash=wcsrchr(path,L'\\'); name=slash?slash+1:path;
    }
    PROCESS_MEMORY_COUNTERS_EX pm{};
    bool ok=GetProcessMemoryInfo(h,(PROCESS_MEMORY_COUNTERS*)&pm,sizeof(pm));
    if(ok) working_set_mb=pm.WorkingSetSize/1048576ull;
    CloseHandle(h); return ok;
}

bool get_foreground_process(unsigned long& pid, std::wstring& name) {
    HWND w = GetForegroundWindow();
    if (!w) return false;
    DWORD p = 0;
    GetWindowThreadProcessId(w, &p);
    if (!p) return false;
    unsigned long long mb = 0;
    pid = p;
    return get_process_info(p, name, mb);
}
bool looks_like_game_process(const std::wstring& name) {
    std::wstring x = name;
    for (auto& c : x) c = (wchar_t)towlower(c);
    const wchar_t* hints[] = {
        L"game", L"steam", L"valorant", L"fortnite", L"eldenring",
        L"cs2", L"csgo", L"gta", L"minecraft", L"roblox", L"pubg",
        L"apex", L"overwatch", L"league", L"dota", L"warzone"
    };
    for (auto h : hints) if (x.find(h) != std::wstring::npos) return true;
    return false;
}

static ULONGLONG za_ft(const FILETIME& f){ULARGE_INTEGER u{};u.LowPart=f.dwLowDateTime;u.HighPart=f.dwHighDateTime;return u.QuadPart;}
bool get_process_cpu_percent(unsigned long pid,double& percent){static ULONGLONG lp=0,ls=0;static DWORD lpid=0;HANDLE hp=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);if(!hp)return false;FILETIME c,e,k,u{};bool ok=GetProcessTimes(hp,&c,&e,&k,&u);CloseHandle(hp);if(!ok)return false;FILETIME idle,sk,su;if(!GetSystemTimes(&idle,&sk,&su))return false;ULONGLONG p=za_ft(k)+za_ft(u),sys=za_ft(sk)+za_ft(su);if(lpid!=pid||!lp||!ls){lpid=pid;lp=p;ls=sys;percent=0;return true;}if (p < lp || sys < ls) { lp=p; ls=sys; percent=0.0; return true; } ULONGLONG dp=p-lp,ds=sys-ls;lp=p;ls=sys;percent=ds?100.0*(double)dp/(double)ds:0;if(percent<0)percent=0;if(percent>100)percent=100;return true;}
bool get_process_thread_count(unsigned long pid,unsigned long& count){HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(snap==INVALID_HANDLE_VALUE)return false;THREADENTRY32 te{};te.dwSize=sizeof(te);count=0;if(Thread32First(snap,&te)){do{if(te.th32OwnerProcessID==pid)++count;}while(Thread32Next(snap,&te));}CloseHandle(snap);return count>0;}

}
