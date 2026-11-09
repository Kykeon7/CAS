
#pragma once

#include <files/serialization/automaton.h>
#include <viewports/viewport.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace cas::files::serialization {

struct viewport_snapshot {
    std::string name;
    automaton_snapshot simulation;

    math::vec2f camera_position;
    float camera_zoom = 1.0f;

    cas::rendering::automaton::config render_config;
    cas::rendering::automaton::config preview_render_config;

    std::vector<float> population_history;
    std::vector<float> density_history;
    std::vector<float> activity_history;

    std::vector<std::uint8_t> linear_history;
};

[[nodiscard]]
viewport_snapshot capture_viewport(
    const cas::viewports::viewport& value
);

[[nodiscard]]
nlohmann::json save_viewport(
    const viewport_snapshot& snapshot
);

[[nodiscard]]
nlohmann::json save_viewport(
    const cas::viewports::viewport& value
);

cas::viewports::viewport load_viewport(
    const nlohmann::json& document,
    progress_reporter progress = {}
);

void save_viewport_file(
    const std::filesystem::path& path,
    const cas::viewports::viewport& value
);

void save_viewport_file(
    const std::filesystem::path& path,
    const viewport_snapshot& snapshot
);

cas::viewports::viewport load_viewport_file(
    const std::filesystem::path& path,
    progress_callback on_progress
);

} // namespace cas::files::serialization
