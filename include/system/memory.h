#pragma once

#include <cstddef>

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

namespace cas::system {

size_t get_memory_usage_bytes() {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return static_cast<size_t>(info.WorkingSetSize);
#elif defined(__unix__) || defined(__APPLE__)
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
#if defined(__APPLE__)
        return static_cast<size_t>(usage.ru_maxrss);
#else
        return static_cast<size_t>(usage.ru_maxrss * 1024ULL);
#endif
    }
    return 0;
#else
    return 0;
#endif
}

float get_memory_usage_mb() {
    return get_memory_usage_bytes() / (1024.0f * 1024.0f);
}

} // namespace cas::system