
#pragma once

#include <nlohmann/json_fwd.hpp>

#include <array>
#include <filesystem>
#include <optional>

#include <files/serialization/progress.h>
#include <files/serialization/viewport.h>
#include <viewports/manager.h>

namespace cas::files::serialization {

/// @brief Stores a reconstructed workspace.
struct workspace_data {
    workspace_data() = default;

    workspace_data(const workspace_data&) = default;
    workspace_data& operator=(const workspace_data&) = default;

    workspace_data(workspace_data&&) = default;
    workspace_data& operator=(workspace_data&&) = default;

    ~workspace_data() = default;

    /// @brief Collection of restored viewports.
    cas::viewports::manager viewports;

    /// @brief Restored application UI scale.
    float ui_scale = 1.0f;
};

/// @brief Stores an independent snapshot of a workspace.
struct workspace_snapshot {
    workspace_snapshot() = default;

    workspace_snapshot(const workspace_snapshot&) = default;
    workspace_snapshot& operator=(const workspace_snapshot&) = default;

    workspace_snapshot(workspace_snapshot&&) = default;
    workspace_snapshot& operator=(workspace_snapshot&&) = default;

    ~workspace_snapshot() = default;

    /// @brief Serialized state of each available viewport.
    std::array<
        std::optional<viewport_snapshot>,
        cas::viewports::manager::max_viewports
    > viewports;

    /// @brief ID of the currently active viewport.
    cas::viewports::manager::viewport_id active_viewport = 0;

    /// @brief Application UI scale.
    float ui_scale = 1.0f;
};

/// @brief Captures the current state of a workspace.
/// @param viewports Viewport manager to capture.
/// @param ui_scale Current application UI scale.
/// @return An independent workspace snapshot.
[[nodiscard]]
workspace_snapshot capture_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
);

/// @brief Serializes a workspace snapshot into a JSON document.
[[nodiscard]]
nlohmann::json save_workspace(
    const workspace_snapshot& snapshot
);

/// @brief Captures and serializes a workspace.
[[nodiscard]]
nlohmann::json save_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
);

/// @brief Reconstructs a workspace from a JSON document.
/// @param document Serialized workspace.
/// @param progress Progress reporter for the reconstruction.
[[nodiscard]]
workspace_data load_workspace(
    const nlohmann::json& document,
    progress_reporter progress = {}
);

/// @brief Saves a workspace snapshot to a file.
void save_workspace_file(
    const std::filesystem::path& path,
    const workspace_snapshot& snapshot
);

/// @brief Captures and saves a workspace to a file.
void save_workspace_file(
    const std::filesystem::path& path,
    const cas::viewports::manager& viewports,
    float ui_scale
);

/// @brief Loads a workspace from a file.
/// @param path Source file.
/// @param on_progress Callback for reporting loading progress.
[[nodiscard]]
workspace_data load_workspace_file(
    const std::filesystem::path& path,
    progress_callback on_progress = {}
);

} // namespace cas::files::serialization
