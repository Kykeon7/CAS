#pragma once

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <files/serialization/automaton.h>
#include <files/serialization/progress.h>
#include <viewports/viewport.h>

namespace cas::files::serialization {

/// @brief Stores an independent snapshot of a viewport.
struct viewport_snapshot {
    viewport_snapshot() = default;

    viewport_snapshot(const viewport_snapshot&) = default;
    viewport_snapshot& operator=(const viewport_snapshot&) = default;

    viewport_snapshot(viewport_snapshot&&) = default;
    viewport_snapshot& operator=(viewport_snapshot&&) = default;

    ~viewport_snapshot() = default;

    /// @brief Viewport display name.
    std::string name;

    /// @brief Snapshot of the underlying automaton.
    automaton_snapshot simulation;

    /// @brief Camera position in world coordinates.
    math::vec2f camera_position;

    /// @brief Camera zoom factor.
    float camera_zoom = 1.0f;

    /// @brief Rendering configuration.
    cas::rendering::automaton::config render_config;

    /// @brief Rendering configuration used for previews.
    cas::rendering::automaton::config preview_render_config;

    /// @brief Historical population measurements.
    std::vector<float> population_history;

    /// @brief Historical density measurements.
    std::vector<float> density_history;

    /// @brief Historical activity measurements.
    std::vector<float> activity_history;

    /// @brief Historical states for linear automata.
    std::vector<std::uint8_t> linear_history;
};

/// @brief Captures the current state of a viewport.
/// @param value Viewport to capture.
/// @return An independent snapshot of the viewport.
[[nodiscard]]
viewport_snapshot capture_viewport(
    const cas::viewports::viewport& value
);

/// @brief Serializes a viewport snapshot into a JSON document.
[[nodiscard]]
nlohmann::json save_viewport(
    const viewport_snapshot& snapshot
);

/// @brief Captures and serializes a viewport.
[[nodiscard]]
nlohmann::json save_viewport(
    const cas::viewports::viewport& value
);

/// @brief Reconstructs a viewport from a JSON document.
/// @param document Serialized viewport.
/// @param progress Progress reporter for the reconstruction.
[[nodiscard]]
cas::viewports::viewport load_viewport(
    const nlohmann::json& document,
    progress_reporter progress = {}
);

/// @brief Saves a viewport to a file.
void save_viewport_file(
    const std::filesystem::path& path,
    const cas::viewports::viewport& value
);

/// @brief Saves a viewport snapshot to a file.
void save_viewport_file(
    const std::filesystem::path& path,
    const viewport_snapshot& snapshot
);

/// @brief Loads a viewport from a file.
/// @param path Source file.
/// @param on_progress Callback for reporting loading progress.
[[nodiscard]]
cas::viewports::viewport load_viewport_file(
    const std::filesystem::path& path,
    progress_callback on_progress
);

} // namespace cas::files::serialization