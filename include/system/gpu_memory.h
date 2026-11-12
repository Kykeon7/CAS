
#pragma once

#include <cstddef>

namespace cas::system {

/**
 * Stores GPU memory capacity, usage, and budget information.
 *
 * Memory capacity values describe the adapter's reported hardware
 * capacity, while usage and budget values describe the current
 * process memory information reported by DXGI.
 *
 * The information is only valid when @ref available is true.
 */
struct gpu_memory_info {
    /** Indicates whether the memory information was retrieved successfully. */
    bool available = false;

    /** Dedicated video memory capacity reported by the adapter, in bytes. */
    std::size_t dedicated_capacity_bytes = 0;

    /** Shared system memory capacity reported by the adapter, in bytes. */
    std::size_t shared_capacity_bytes = 0;

    /** Current process usage of local video memory, in bytes. */
    std::size_t local_used_bytes = 0;

    /** Current process budget for local video memory, in bytes. */
    std::size_t local_budget_bytes = 0;

    /** Current process usage of non-local memory, in bytes. */
    std::size_t non_local_used_bytes = 0;

    /** Current process budget for non-local memory, in bytes. */
    std::size_t non_local_budget_bytes = 0;
};

/**
 * Retrieves GPU memory information for the adapter matching an SDL GPU device.
 *
 * The adapter is selected by matching the supplied SDL device name
 * against DXGI adapter descriptions. Information is returned only
 * when exactly one adapter matches.
 *
 * @param sdl_device_name Name of the selected SDL GPU device.
 * @return Retrieved memory information. If the adapter cannot be
 *         identified uniquely or the DXGI query fails, @ref available
 *         is false and the remaining fields retain their default values.
 */
[[nodiscard]]
gpu_memory_info get_gpu_memory_info(
    const char* sdl_device_name
);

} // namespace cas::system
