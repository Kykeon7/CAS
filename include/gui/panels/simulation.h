#pragma once

#include <functional>

#include <application/settings.h>
#include <gui/panels/panel.h>
#include <gui/rule_editor.h>
#include <gui/settings_tab.h>
#include <gui/viewport_view.h>

namespace cas::gui::panels {

/**
 * @brief Provides controls for configuring and managing a simulation.
 */
class simulation final : public panel {
public:
    /**
     * @brief Constructs a simulation panel.
     *
     * @param view Viewport associated with the simulation.
     * @param settings Application settings.
     * @param open_settings Callback used to open a settings tab.
     */
    explicit simulation(
        viewport_view& view,
        settings& settings,
        std::function<void(settings_tab)> open_settings
    );

    simulation(const simulation&) = delete;
    simulation& operator=(const simulation&) = delete;

    simulation(simulation&&) = delete;
    simulation& operator=(simulation&&) = delete;

    ~simulation() override = default;

    /**
     * @brief Renders the simulation controls and configuration.
     */
    void render() override;

private:
    void render_controls(cas::simulations::simulation& sim);
    void render_size(cas::simulations::simulation& sim);
    void render_resize_popup(cas::simulations::simulation& sim);
    void render_speed(cas::simulations::simulation& sim);
    void render_rule(cas::simulations::simulation& sim);

private:
    viewport_view& _view;
    rule_editor _rule_editor;
    std::function<void(settings_tab)> _open_settings;

    int _temp_width = 1;
    int _temp_height = 1;

    settings& _settings;
};

} // namespace cas::gui::panels