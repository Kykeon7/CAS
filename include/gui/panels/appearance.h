
#pragma once

#include <gui/panels/panel.h>
#include <gui/viewport_view.h>

namespace cas::gui::panels {

class appearance final : public panel {
public:
    explicit appearance(viewport_view& view);

    void render() override;

private:
    void render_colors();
    void render_layout();
    void render_preview() const;
    void render_actions();

    viewport_view& _view;
};

} // namespace cas::gui::panels
