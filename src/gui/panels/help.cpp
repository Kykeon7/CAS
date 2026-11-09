#include <gui/panels/help.h>

#include <gui/help_content.h>

#include <imgui.h>

namespace cas::gui::panels {

void help::render() {
    ImGui::Text("Help");

    ImGui::PushItemWidth(-1);

    ImGui::BeginChild(
        "HelpContent",
        ImGui::GetContentRegionAvail()
    );

    render_help_content();

    ImGui::EndChild();

    ImGui::PopItemWidth();
}

} // namespace cas::gui::panels