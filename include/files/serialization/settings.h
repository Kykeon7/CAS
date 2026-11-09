#pragma once

#include <application/settings.h>

#include <nlohmann/json_fwd.hpp>

#include <filesystem>

namespace cas::files::serialization {

[[nodiscard]]
nlohmann::json save_application_settings(
    const settings& settings
);

[[nodiscard]]
settings load_application_settings(
    const nlohmann::json& document
);

void save_application_settings_file(
    const std::filesystem::path& path,
    const settings& settings
);

[[nodiscard]]
settings load_application_settings_file(
    const std::filesystem::path& path
);

} // namespace cas::files::serialization