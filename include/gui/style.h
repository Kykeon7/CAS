
#pragma once

#include <application/settings.h>

namespace cas::gui {

/**
 * @brief Applies the configured interface style to Dear ImGui.
 *
 * Updates the current ImGui style and global font scale.
 *
 * @param settings Interface settings to apply.
 *
 * @throws std::runtime_error If the theme or density is invalid.
 */
void apply_interface_style(const interface_settings& settings);

} // namespace cas::gui
