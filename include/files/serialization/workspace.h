
#pragma once

#include <files/serialization/viewport.h>
#include <viewports/manager.h>

#include <nlohmann/json_fwd.hpp>

#include <array>
#include <filesystem>
#include <optional>

#include <files/serialization/progress.h>

namespace cas::files::serialization {

struct workspace_data {
    cas::viewports::manager viewports;
    float ui_scale = 1.0f;
};

struct workspace_snapshot {
    std::array<
        std::optional<viewport_snapshot>,
        cas::viewports::manager::max_viewports
    > viewports;

    cas::viewports::manager::viewport_id active_viewport = 0;
    float ui_scale = 1.0f;
};

[[nodiscard]]
workspace_snapshot capture_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
);

[[nodiscard]]
nlohmann::json save_workspace(
    const workspace_snapshot& snapshot
);

[[nodiscard]]
nlohmann::json save_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
);

[[nodiscard]]
workspace_data load_workspace(
    const nlohmann::json& document,
    progress_reporter progress = {}
);


void save_workspace_file(
    const std::filesystem::path& path,
    const workspace_snapshot& snapshot
);

void save_workspace_file(
    const std::filesystem::path& path,
    const cas::viewports::manager& viewports,
    float ui_scale
);

[[nodiscard]]
workspace_data load_workspace_file(
    const std::filesystem::path& path,
    progress_callback on_progress = {}
);

} // namespace cas::files::serialization
