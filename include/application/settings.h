#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

#include <rendering/backend.h>

namespace cas {

enum class ui_theme {
    dark,
    light,
    classic
};

enum class ui_density {
    compact,
    comfortable,
    spacious
};

struct stats_panel_settings {
    bool show_performance = true;
    bool show_memory = true;
    bool show_simulation = true;
    bool show_gpu = true;
};

struct interface_settings {
    ui_theme theme = ui_theme::dark;
    ui_density density = ui_density::comfortable;
    float scale = 1.0f;
};

struct rendering_settings {
    rendering::backend backend = rendering::backend::gpu;
};

struct randomization_settings {
    std::array<std::uint32_t, 2> two_states{ 50, 50 };
    std::array<std::uint32_t, 3> three_states{ 34, 33, 33 };

    std::uint32_t seed = 5489;
};

struct settings {
    std::filesystem::path default_save_directory;

    std::string workspace_filename = "workspace.json";

    std::filesystem::path last_workspace_path;

    bool load_workspace_on_startup = true;
    bool save_workspace_on_exit = true;
    bool save_imgui_layout = true;

    rendering_settings rendering;
    randomization_settings randomization;
    interface_settings interface;
    stats_panel_settings stats;
};

} // namespace cas