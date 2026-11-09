#include <gui/help_content.h>

#include <imgui.h>

namespace cas::gui {

void render_help_content() {
    ImGui::Text("Controls");
    ImGui::Separator();

    ImGui::BulletText("SPACE: Start / Pause");
    ImGui::BulletText("R: Randomize");
    ImGui::BulletText("ESC: Exit");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("About");
    ImGui::Separator();

    ImGui::TextWrapped(
        "CAS (Cellular Automaton Simulator) is an academic "
        "project developed for educational purposes."
    );

    ImGui::TextWrapped(
        "This project explores cellular automata, simulation, "
        "and real-time rendering using C++."
    );

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Authors");
    ImGui::Separator();

    ImGui::BulletText("Eleanor Esparza");
    ImGui::BulletText("Juan Rugerio");
    ImGui::BulletText("Leonardo Perez");
}

} // namespace cas