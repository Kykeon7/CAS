#pragma once

#include <gui/panels/panel.h>

namespace cas::gui::panels {

/// @brief Displays the application's help information.
class help final : public panel {
public:
    help() = default;
    help(const help&) = default;
    help& operator=(const help&) = default;
    help(help&&) noexcept = default;
    help& operator=(help&&) noexcept = default;
    ~help() override = default;

    /// @brief Renders the help panel.
    void render() override;
};

} // namespace cas::gui::panels
