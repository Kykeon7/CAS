#pragma once

#include <windows.h>
#include <psapi.h>

namespace cas::system {

size_t get_memory_usage_bytes() {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return info.WorkingSetSize;
}		

float get_memory_usage_mb() {
    return get_memory_usage_bytes() / (1024.0f * 1024.0f);
}

} // namespace cas::system