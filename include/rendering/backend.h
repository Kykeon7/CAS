#pragma once

namespace cas::rendering {

/**
 * Identifies the rendering backend used by the application.
 */
enum class backend {
    /** CPU-based rendering backend. */
    cpu,

    /** GPU-based rendering backend. */
    gpu
};

} // namespace cas::rendering