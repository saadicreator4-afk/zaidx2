#pragma once
#include <string>
namespace zaidx {
struct SystemInfo {
    unsigned int logical_cpus{};
    unsigned long long total_ram_mb{};
    unsigned long long available_ram_mb{};
    std::wstring gpu_name;
    std::wstring gpu_vendor;
    std::wstring os_name;
    double cpu_load_percent{};
};
SystemInfo query_system_info();
std::wstring get_process_list_text();
std::wstring get_gpu_list_text();
std::wstring get_process_list_items();
bool get_process_info(unsigned long pid, std::wstring& name, unsigned long long& working_set_mb);
bool get_foreground_process(unsigned long& pid, std::wstring& name);
bool looks_like_game_process(const std::wstring& name);
bool get_process_cpu_percent(unsigned long pid, double& percent);
bool get_process_thread_count(unsigned long pid, unsigned long& count);
}
