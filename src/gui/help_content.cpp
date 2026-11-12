
#include <gui/help_content.h>

#include <imgui.h>

namespace cas::gui {

void render_help_content() {
    ImGui::Text("Getting Started");
    ImGui::Separator();

    ImGui::TextWrapped(
        "Follow these steps to create and run a simple simulation:"
    );

    ImGui::Spacing();

    ImGui::Text("1. Initialize the automaton");
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "Press CTRL + SHIFT + R to randomize the cell states "
        "and generate an initial configuration."
    );

    ImGui::Spacing();

    ImGui::Text("2. Edit the cell states");
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "Click individual cells to change their states "
        "and customize the initial configuration."
    );

    ImGui::Spacing();

    ImGui::Text("3. Start the simulation");
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "Press CTRL + SHIFT + SPACE to start the simulation "
        "and observe how the automaton evolves over time."
    );

    ImGui::Spacing();

    ImGui::Text("4. Pause and experiment");
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "Press CTRL + SHIFT + SPACE again to pause the simulation. "
        "You can then modify the cell states and try "
        "different initial configurations."
    );

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Controls");
    ImGui::Separator();

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "CTRL + SHIFT + SPACE: Start / Pause simulation"
    );

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "CTRL + SHIFT + R: Randomize cell states (Custom)"
    );

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "CTRL + SHIFT + Q: Exit application"
    );

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped(
        "Mouse click: Change a cell state"
    );

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("About");
    ImGui::Separator();

    ImGui::TextWrapped(
        "CAS (Cellular Automaton Simulator) is an academic "
        "project developed for educational purposes."
    );

    ImGui::Spacing();

    ImGui::TextWrapped(
        "The application provides an environment for exploring "
        "cellular automata through interactive editing, "
        "simulation, and real-time visualization."
    );

    ImGui::Spacing();

    ImGui::TextWrapped(
        "Users can experiment with different initial cell "
        "configurations, observe their evolution, and inspect "
        "simulation data through graphical representations."
    );

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Authors");
    ImGui::Separator();

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped("Eleanor Esparza");

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped("Juan Rugerio");

    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextWrapped("Leonardo Perez");
}

} // namespace cas::gui
