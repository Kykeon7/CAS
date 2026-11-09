#pragma once

#include <gui/panels/panel.h>
#include <gui/rule_editor.h>
#include <gui/settings_tab.h>
#include <gui/viewport_view.h>
#include <application/settings.h>
#include <functional>

namespace cas::gui::panels {

class simulation final : public panel {
public:
    explicit simulation(
        viewport_view& view,
        settings& settings,
        std::function<void(settings_tab)> open_settings
    );


    void render() override;

private:
    void render_controls(cas::simulations::simulation& sim);
    void render_size(cas::simulations::simulation& sim);
    void render_resize_popup(cas::simulations::simulation& sim);
    void render_speed(cas::simulations::simulation& sim);
    void render_rule(cas::simulations::simulation& sim);

    viewport_view& _view;
    rule_editor _rule_editor;
    std::function<void(settings_tab)> _open_settings;

    int _temp_width = 1;
    int _temp_height = 1;

    settings& _settings;
};

} // namespace cas::gui::panels