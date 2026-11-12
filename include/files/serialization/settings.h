#pragma once

#include <nlohmann/json_fwd.hpp>

#include <filesystem>

#include <application/settings.h>

namespace cas::files::serialization {

/// @brief Serializes application settings into a JSON document.
[[nodiscard]]
nlohmann::json save_application_settings(
    const settings& settings
);

/// @brief Restores application settings from a JSON document.
[[nodiscard]]
settings load_application_settings(
    const nlohmann::json& document
);

/// @brief Saves application settings to a JSON file.
void save_application_settings_file(
    const std::filesystem::path& path,
    const settings& settings
);

/// @brief Loads application settings from a JSON file.
[[nodiscard]]
settings load_application_settings_file(
    const std::filesystem::path& path
);

} // namespace cas::files::serialization