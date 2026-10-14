#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <powrprof.h>
#include <filesystem>
#include <sstream>
#include "optimizer.h"

#pragma comment(lib, "powrprof.lib")
namespace fs = std::filesystem;
namespace zaidx {

bool clean_user_temp(std::wstring& report) {
    wchar_t buf[MAX_PATH]{};
    DWORD n = GetTempPathW(MAX_PATH, buf);
    if (!n || n >= MAX_PATH) { report=L"Could not locate user TEMP."; return false; }
    size_t removed=0, failed=0;
    std::error_code ec;
    fs::path root(buf);
    for (auto it = fs::directory_iterator(root, fs::directory_options::skip_permission_denied, ec);
         it != fs::directory_iterator(); it.increment(ec)) {
        if (ec) { ++failed; ec.clear(); continue; }
        std::error_code rmec;
        fs::remove_all(it->path(), rmec);
        if (rmec) ++failed; else ++removed;
    }
    std::wstringstream ss;
    ss << L"TEMP cleanup complete. Removed entries: " << removed
       << L", skipped/failed: " << failed;
    report=ss.str();
    return true;
}

bool set_current_process_high_priority(std::wstring& report) {
    if (SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS)) {
        report=L"ZAIDX process priority set to High.";
        return true;
    }
    report=L"Could not change ZAIDX process priority.";
    return false;
}

static bool set_process_priority(unsigned long pid, DWORD cls, std::wstring& report, const wchar_t* ok) {
    // Avoid invalid and especially sensitive targets. ZAIDX has its own dedicated
    // button for changing its own priority, so the selected-process API rejects it.
    if (pid == 0) { report=L"PID 0 is not a valid target for this action."; return false; }
    if (pid == 4) { report=L"System process priority changes are blocked for safety."; return false; }
    if (pid == GetCurrentProcessId()) {
        report=L"Selected-process priority changes are blocked for ZAIDX itself. Use the ZAIDX Priority button.";
        return false;
    }

    HANDLE h=OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) { report=L"Could not open selected process. It may have exited or require an elevated session."; return false; }

    // Confirm the opened handle still identifies the requested process.
    DWORD actualPid = GetProcessId(h);
    if (!actualPid || actualPid != pid) {
        CloseHandle(h);
        report=L"Target process changed or is no longer available. Refresh and select it again.";
        return false;
    }

    BOOL result=SetPriorityClass(h, cls);
    DWORD error = result ? ERROR_SUCCESS : GetLastError();
    CloseHandle(h);
    if (result) {
        report=ok;
        return true;
    }
    std::wstringstream ss;
    ss << L"Could not change selected process priority (Windows error " << error << L").";
    report=ss.str();
    return false;
}

bool set_process_high_priority(unsigned long pid, std::wstring& report) {
    return set_process_priority(pid, HIGH_PRIORITY_CLASS, report, L"Selected process priority set to High.");
}

bool set_process_normal_priority(unsigned long pid, std::wstring& report) {
    return set_process_priority(pid, NORMAL_PRIORITY_CLASS, report, L"Selected process priority restored to Normal.");
}

bool set_high_performance_plan(std::wstring& report) {
    GUID* active=nullptr;
    GUID high{};
    bool found=false;
    if (PowerGetActiveScheme(nullptr, &active) != ERROR_SUCCESS) active=nullptr;
    DWORD index=0;
    DWORD cb=sizeof(GUID);
    while (PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, index, reinterpret_cast<UCHAR*>(&high), &cb) == ERROR_SUCCESS) {
        cb=sizeof(GUID);
        wchar_t name[256]{}; DWORD size=sizeof(name);
        if (PowerReadFriendlyName(nullptr, &high, nullptr, nullptr, reinterpret_cast<UCHAR*>(name), &size) == ERROR_SUCCESS) {
            std::wstring n=name;
            if (n.find(L"High performance") != std::wstring::npos || n.find(L"Ultimate Performance") != std::wstring::npos) { found=true; break; }
        }
        ++index;
    }
    if (found && PowerSetActiveScheme(nullptr, &high) == ERROR_SUCCESS) {
        report=L"High Performance power plan activated.";
        if (active) LocalFree(active);
        return true;
    }
    if (active) LocalFree(active);
    report=L"High Performance plan was not changed (not found or permission denied).";
    return false;
}

std::wstring get_safe_mode_status() {
    HKEY h=nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Option",0,KEY_READ,&h)==ERROR_SUCCESS) {
        DWORD v=0, cb=sizeof(v), type=0;
        if (RegQueryValueExW(h,L"OptionValue",nullptr,&type,reinterpret_cast<BYTE*>(&v),&cb)==ERROR_SUCCESS) {
            RegCloseKey(h);
            return v ? L"Safe Mode state detected." : L"Normal Windows boot.";
        }
        RegCloseKey(h);
    }
    return L"Safe Mode state could not be determined.";
}

std::wstring get_active_power_plan() {
    GUID* active=nullptr;
    if (PowerGetActiveScheme(nullptr, &active) != ERROR_SUCCESS || !active)
        return L"Power plan: unavailable";
    wchar_t name[256]{};
    DWORD size=256;
    std::wstring result=L"Power plan: active (GUID detected)";
    if (PowerReadFriendlyName(nullptr, active, nullptr, nullptr, reinterpret_cast<UCHAR*>(name), &size) == ERROR_SUCCESS && name[0])
        result=std::wstring(L"Power plan: ") + name;
    LocalFree(active);
    return result;
}
}
