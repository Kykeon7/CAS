
#pragma once

#include <gui/panels/panel.h>

namespace cas::gui::panels {

class help final : public panel {
public:
    help() = default;

    void render() override;
};

} // namespace cas::gui::panels
