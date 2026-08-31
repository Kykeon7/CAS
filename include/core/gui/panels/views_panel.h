#pragma once

namespace cas::panels {
class ViewportsPanel : public Panel {
public:
    ViewportsPanel(std::vector<std::unique_ptr<Viewport>>& v, size_t& active)
        : m_viewports(v), m_active(active) {
    }

    void render() override {
        ImGui::Text("Viewports");

        ImGui::PushItemWidth(-1);

        ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());

        if (ImGui::BeginTabBar("ViewportsTabs")) {

            for (size_t i = 0; i < m_viewports.size(); ++i) {
                std::string name = "Viewport " + std::to_string(i);

                if (ImGui::BeginTabItem(name.c_str())) {

                    if (m_active != i)
                        m_active = i;

                    ImGui::EndTabItem();
                }
            }

            // botón "+"
            if (ImGui::TabItemButton("+")) {
                m_viewports.emplace_back(std::make_unique<Viewport>());
            }

            ImGui::EndTabBar();
        }

        ImGui::EndChild();

        ImGui::PopItemWidth();
    }

private:
    std::vector<std::unique_ptr<Viewport>>& m_viewports;
    size_t& m_active;
};  
} // namespace cas::panels