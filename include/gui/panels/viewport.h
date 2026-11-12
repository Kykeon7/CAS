#pragma once

#include <gui/panels/panel.h>
#include <inputs/input.h>
#include <viewports/manager.h>

namespace cas::gui::panels {

/**
 * @brief Panel responsible for displaying and interacting with viewports.
 *
 * Provides viewport selection, zoom controls, rendering, and input handling.
 */
class viewport final : public panel {
public:
    /**
     * @brief Constructs the viewport panel.
     *
     * @param viewports Viewport manager.
     * @param input Input system.
     */
    viewport(
        cas::viewports::manager& viewports,
        const inputs::input& input
    );

    viewport(const viewport&) = delete;
    viewport& operator=(const viewport&) = delete;

    viewport(viewport&&) = delete;
    viewport& operator=(viewport&&) = delete;

    ~viewport() override = default;

    /**
     * @brief Renders the viewport panel and handles its input.
     */
    void render() override;

private:
    cas::viewports::manager& _viewports;
    const inputs::input& _input;
};

} // namespace cas::gui::panels