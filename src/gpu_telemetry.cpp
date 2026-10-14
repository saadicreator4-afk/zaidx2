#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cwchar>
#include "gpu_telemetry.h"
#pragma comment(lib, "pdh.lib")

bool query_gpu_utilization(double& percent) {
    percent = 0.0;
    static PDH_HQUERY query = nullptr;
    static std::vector<PDH_HCOUNTER> counters;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        if (PdhOpenQueryW(nullptr, 0, &query) != ERROR_SUCCESS) {
            initialized = false;
            return false;
        }
        DWORD chars = 0;
        PDH_STATUS st = PdhExpandWildCardPathW(nullptr,
            L"\\GPU Engine(*)\\Utilization Percentage", nullptr, &chars, 0);
        if (st != PDH_MORE_DATA || chars == 0) {
            PdhCloseQuery(query);
            query = nullptr;
            initialized = false;
            return false;
        }
        std::vector<wchar_t> paths(chars + 2, L'\0');
        st = PdhExpandWildCardPathW(nullptr,
            L"\\GPU Engine(*)\\Utilization Percentage", paths.data(), &chars, 0);
        if (st != ERROR_SUCCESS) {
            PdhCloseQuery(query);
            query = nullptr;
            initialized = false;
            return false;
        }
        for (const wchar_t* path = paths.data(); *path; path += wcslen(path) + 1) {
            PDH_HCOUNTER c{};
            if (PdhAddEnglishCounterW(query, path, 0, &c) == ERROR_SUCCESS)
                counters.push_back(c);
        }
        if (counters.empty()) {
            PdhCloseQuery(query);
            query = nullptr;
            initialized = false;
            return false;
        }
        if (PdhCollectQueryData(query) != ERROR_SUCCESS) {
            for (auto c : counters) PdhRemoveCounter(c);
            counters.clear();
            PdhCloseQuery(query);
            query = nullptr;
            initialized = false;
            return false;
        } // establish baseline
        return false;
    }
    if (!query || counters.empty()) {
        initialized = false;
        return false;
    }
    if (PdhCollectQueryData(query) != ERROR_SUCCESS) return false;
    double total = 0.0;
    bool any = false;
    for (auto c : counters) {
        PDH_FMT_COUNTERVALUE v{};
        if (PdhGetFormattedCounterValue(c, PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS &&
            v.CStatus == PDH_CSTATUS_VALID_DATA) {
            total += std::max(0.0, v.doubleValue);
            any = true;
        }
    }
    if (!any) return false;
    // GPU Engine counters are per engine; sum is a useful activity indicator,
    // capped at 100% for a simple dashboard scale.
    percent = std::min(100.0, total);
    return true;
}
