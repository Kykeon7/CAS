
#pragma once

#include <gui/panels/panel.h>
#include <inputs/input.h>
#include <viewports/manager.h>

namespace cas::gui::panels {

class viewport final : public panel {
public:
    viewport(
        cas::viewports::manager& viewports,
        const inputs::input& input
    );

    void render() override;

private:
    cas::viewports::manager& _viewports;
    const inputs::input& _input;
};

} // namespace cas::gui::panels
