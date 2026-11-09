#include <files/serialization/settings.h>

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <cmath>

namespace cas::files::serialization {
namespace {

using json = nlohmann::json;

constexpr int schema_version = 1;

void validate_workspace_filename(
    std::string_view filename
) {
    if (filename.empty()
        || filename == "."
        || filename == ".."
        || filename.find('/') != std::string_view::npos
        || filename.find('\\') != std::string_view::npos) {
        throw std::runtime_error(
            "Invalid workspace filename"
        );
    }
}

void validate_randomization(
    const randomization_settings& settings
) {
    const std::uint64_t two_states =
        static_cast<std::uint64_t>(settings.two_states[0]) +
        settings.two_states[1];

    const std::uint64_t three_states =
        static_cast<std::uint64_t>(settings.three_states[0]) +
        settings.three_states[1] +
        settings.three_states[2];

    if (two_states != 100 || three_states != 100) {
        throw std::runtime_error(
            "Randomization percentages must sum to 100"
        );
    }
}

std::string_view to_string(ui_theme theme) {
    switch (theme) {
    case ui_theme::dark:    return "dark";
    case ui_theme::light:   return "light";
    case ui_theme::classic: return "classic";
    }

    throw std::runtime_error("Invalid UI theme");
}

std::string_view to_string(ui_density density) {
    switch (density) {
    case ui_density::compact:     return "compact";
    case ui_density::comfortable: return "comfortable";
    case ui_density::spacious:    return "spacious";
    }

    throw std::runtime_error("Invalid UI density");
}

ui_theme ui_theme_from_string(const std::string& value) {
    if (value == "dark")    return ui_theme::dark;
    if (value == "light")   return ui_theme::light;
    if (value == "classic") return ui_theme::classic;

    throw std::runtime_error("Invalid UI theme");
}

ui_density ui_density_from_string(const std::string& value) {
    if (value == "compact")     return ui_density::compact;
    if (value == "comfortable") return ui_density::comfortable;
    if (value == "spacious")    return ui_density::spacious;

    throw std::runtime_error("Invalid UI density");
}

void validate_interface(const interface_settings& settings) {
    if (!std::isfinite(settings.scale)
        || settings.scale < 0.8f
        || settings.scale > 1.2f) {
        throw std::runtime_error("Invalid UI scale");
    }

    // También valida que los valores de los enums sean válidos.
    to_string(settings.theme);
    to_string(settings.density);
}


void validate_settings(
    const settings& settings
) {
    validate_workspace_filename(
        settings.workspace_filename
    );

    validate_randomization(settings.randomization);

    validate_interface(settings.interface);
}

} // namespace

json save_application_settings(
    const settings& settings
) {
    validate_settings(settings);

    return {
        { "schema_version", schema_version },
        {
            "default_save_directory",
            settings.default_save_directory.string()
        },
        {
            "workspace_filename",
            settings.workspace_filename
        },
        {
            "last_workspace_path",
            settings.last_workspace_path.string()
        },
        {
            "load_workspace_on_startup",
            settings.load_workspace_on_startup
        },
        {
            "save_workspace_on_exit",
            settings.save_workspace_on_exit
        },
        {
            "save_imgui_layout",
            settings.save_imgui_layout
        },
        {
            "randomization", {
                {
                    "two_states",
                    settings.randomization.two_states
                },
                {
                    "three_states",
                    settings.randomization.three_states
                },
                {
                    "seed",
                    settings.randomization.seed
                }
            }
        },
        {
            "interface", {
                { "theme", to_string(settings.interface.theme) },
                { "density", to_string(settings.interface.density) },
                { "scale", settings.interface.scale }
            }
        },
        {
            "stats", {
                { "show_performance", settings.stats.show_performance },
                { "show_memory", settings.stats.show_memory },
                { "show_simulation", settings.stats.show_simulation },
                { "show_gpu", settings.stats.show_gpu }
            }
        }
    };
}

settings load_application_settings(
    const json& document
) {
    if (!document.is_object()) {
        throw std::runtime_error(
            "Application settings document must be an object"
        );
    }

    if (document.at("schema_version").get<int>()
        != schema_version) {
        throw std::runtime_error(
            "Unsupported application settings schema version"
        );
    }

    settings result;

    if (document.contains("default_save_directory")) {
        result.default_save_directory =
            document.at("default_save_directory").get<std::string>();
    }

    if (document.contains("workspace_filename")) {
        result.workspace_filename =
            document.at("workspace_filename").get<std::string>();
    }

    if (document.contains("last_workspace_path")) {
        result.last_workspace_path =
            document.at("last_workspace_path").get<std::string>();
    }

    if (document.contains("load_workspace_on_startup")) {
        result.load_workspace_on_startup =
            document.at("load_workspace_on_startup").get<bool>();
    }

    if (document.contains("save_workspace_on_exit")) {
        result.save_workspace_on_exit =
            document.at("save_workspace_on_exit").get<bool>();
    }

    if (document.contains("save_imgui_layout")) {
        result.save_imgui_layout =
            document.at("save_imgui_layout").get<bool>();
    }

    if (document.contains("randomization")) {
        const auto& randomization = document.at("randomization");

        if (randomization.contains("two_states")) {
            result.randomization.two_states =
                randomization.at("two_states")
                .get<std::array<std::uint32_t, 2>>();
        }

        if (randomization.contains("three_states")) {
            result.randomization.three_states =
                randomization.at("three_states")
                .get<std::array<std::uint32_t, 3>>();
        }

        if (randomization.contains("seed")) {
            result.randomization.seed =
                randomization.at("seed").get<std::uint32_t>();
        }
    }

    if (document.contains("interface")) {
        const auto& interface = document.at("interface");

        if (interface.contains("theme")) {
            result.interface.theme = ui_theme_from_string(
                interface.at("theme").get<std::string>()
            );
        }

        if (interface.contains("density")) {
            result.interface.density = ui_density_from_string(
                interface.at("density").get<std::string>()
            );
        }

        if (interface.contains("scale")) {
            result.interface.scale =
                interface.at("scale").get<float>();
        }
    }

    if (document.contains("stats")) {
        const auto& stats = document.at("stats");

        if (stats.contains("show_performance")) {
            result.stats.show_performance =
                stats.at("show_performance").get<bool>();
        }

        if (stats.contains("show_memory")) {
            result.stats.show_memory =
                stats.at("show_memory").get<bool>();
        }

        if (stats.contains("show_simulation")) {
            result.stats.show_simulation =
                stats.at("show_simulation").get<bool>();
        }

        if (stats.contains("show_gpu")) {
            result.stats.show_gpu =
                stats.at("show_gpu").get<bool>();
        }
    }

    validate_settings(result);

    return result;
}

void save_application_settings_file(
    const std::filesystem::path& path,
    const settings& settings
) {
    const json document =
        save_application_settings(settings);

    std::ofstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open application settings file for writing: "
            + path.string()
        );
    }

    file << document.dump(4) << '\n';

    if (!file) {
        throw std::runtime_error(
            "Failed to write application settings file: "
            + path.string()
        );
    }
}

settings load_application_settings_file(
    const std::filesystem::path& path
) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open application settings file for reading: "
            + path.string()
        );
    }

    const json document = json::parse(file);

    return load_application_settings(document);
}

} // namespace cas::serialization