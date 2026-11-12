#include <gui/panels/help.h>

#include <imgui.h>

#include <gui/help_content.h>

namespace cas::gui::panels {

void help::render() {
    ImGui::Text("Help");
    ImGui::Separator();

    if (ImGui::BeginChild(
        "HelpContent",
        ImGui::GetContentRegionAvail()
    )) {
        render_help_content();
    }

    ImGui::EndChild();
}

} // namespace cas::gui::panels
