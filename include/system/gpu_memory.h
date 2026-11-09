
#pragma once

#include <cstddef>

namespace cas::system {

struct gpu_memory_info {
    bool available = false;

    // Adapter capacity, not current usage.
    std::size_t dedicated_capacity_bytes = 0;
    std::size_t shared_capacity_bytes = 0;

    // DXGI process usage and budgets.
    std::size_t local_used_bytes = 0;
    std::size_t local_budget_bytes = 0;

    std::size_t non_local_used_bytes = 0;
    std::size_t non_local_budget_bytes = 0;
};

// sdl_device_name should come from the selected SDL_GPUDevice.
gpu_memory_info get_gpu_memory_info(
    const char* sdl_device_name
);

} // namespace cas::system
