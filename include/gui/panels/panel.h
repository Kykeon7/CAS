#pragma once

namespace cas::gui::panels {

/// @brief Defines the interface for renderable GUI panels.
class panel {
public:
    panel() = default;

    panel(const panel&) = default;
    panel& operator=(const panel&) = default;

    panel(panel&&) noexcept = default;
    panel& operator=(panel&&) noexcept = default;

    virtual ~panel() = default;

    /// @brief Renders the panel.
    virtual void render() = 0;
};

} // namespace cas::gui::panels