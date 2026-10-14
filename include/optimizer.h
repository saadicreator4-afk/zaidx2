#pragma once
#include <string>

namespace zaidx {
bool clean_user_temp(std::wstring& report);
bool set_current_process_high_priority(std::wstring& report);
bool set_process_high_priority(unsigned long pid, std::wstring& report);
bool set_process_normal_priority(unsigned long pid, std::wstring& report);
std::wstring get_safe_mode_status();
std::wstring get_active_power_plan();
bool set_high_performance_plan(std::wstring& report);
}
