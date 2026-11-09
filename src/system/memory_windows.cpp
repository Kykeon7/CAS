
#include <system/memory.h>

#define NOMINMAX
#include <windows.h>
#include <psapi.h>

namespace cas::system {

std::size_t get_memory_usage_bytes() {
    PROCESS_MEMORY_COUNTERS info{};

    if (!GetProcessMemoryInfo(
        GetCurrentProcess(),
        &info,
        sizeof(info)
    )) {
        return 0;
    }

    return static_cast<std::size_t>(info.WorkingSetSize);
}

float get_memory_usage_mb() {
    return static_cast<float>(
        get_memory_usage_bytes() / (1024.0 * 1024.0)
    );
}

std::size_t get_private_usage_bytes() {
    PROCESS_MEMORY_COUNTERS_EX info{};

    if (!GetProcessMemoryInfo(
        GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&info),
        sizeof(info)
    )) {
        return 0;
    }

    return static_cast<std::size_t>(info.PrivateUsage);
}

float get_private_usage_mb() {
    return static_cast<float>(
        get_private_usage_bytes() / (1024.0 * 1024.0)
    );
}

} // namespace cas::system
