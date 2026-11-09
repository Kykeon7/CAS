#include "gui/panels/container.h"

#include <imgui.h>

#include <utility>

namespace cas::gui::panels {

void container::render(bool& visible) {
    if (!visible) {
        return;
    }

    if (!ImGui::Begin(_name.c_str(), &visible)) {
        ImGui::End();
        return;
    }

    for (std::size_t i = 0; i < _panels.size(); ++i) {
        _panels[i].get().render();

        if (i + 1 < _panels.size()) {
            ImGui::Separator();
        }
    }

    ImGui::End();
}

} // cas::gui::panels