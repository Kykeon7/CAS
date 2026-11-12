#pragma once

#include <gui/panels/panel.h>
#include <gui/viewport_view.h>

namespace cas::gui::panels {

/// @brief Provides controls for viewport appearance settings.
class appearance final : public panel {
public:
    explicit appearance(viewport_view& view);

    appearance(const appearance&) = delete;
    appearance& operator=(const appearance&) = delete;

    appearance(appearance&&) = delete;
    appearance& operator=(appearance&&) = delete;

    ~appearance() override = default;

    /// @brief Renders the appearance panel.
    void render() override;

private:
    void render_colors();
    void render_layout();
    void render_preview() const;
    void render_actions();

private:
    viewport_view& _view;
};

} // namespace cas::gui::panels