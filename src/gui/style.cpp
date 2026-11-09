#include <gui/style.h>

#include <imgui.h>

#include <stdexcept>

namespace cas::gui {
namespace {

constexpr float base_font_scale = 1.25f;

void apply_theme(ImGuiStyle& style, ui_theme theme) {
    switch (theme) {
    case ui_theme::dark:
        ImGui::StyleColorsDark(&style);
        break;

    case ui_theme::light:
        ImGui::StyleColorsLight(&style);
        break;

    case ui_theme::classic:
        ImGui::StyleColorsClassic(&style);
        break;

    default:
        throw std::runtime_error("Invalid UI theme");
    }
}

void apply_density(ImGuiStyle& style, ui_density density) {
    switch (density) {
    case ui_density::compact:
        style.WindowPadding = ImVec2(6.0f, 6.0f);
        style.FramePadding = ImVec2(4.0f, 2.0f);
        style.ItemSpacing = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.CellPadding = ImVec2(4.0f, 2.0f);
        style.IndentSpacing = 16.0f;
        style.ScrollbarSize = 12.0f;
        style.GrabMinSize = 10.0f;
        break;

    case ui_density::comfortable:
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(4.0f, 3.0f);
        style.ItemSpacing = ImVec2(8.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.CellPadding = ImVec2(4.0f, 2.0f);
        style.IndentSpacing = 21.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 12.0f;
        break;

    case ui_density::spacious:
        style.WindowPadding = ImVec2(12.0f, 12.0f);
        style.FramePadding = ImVec2(6.0f, 5.0f);
        style.ItemSpacing = ImVec2(10.0f, 8.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.CellPadding = ImVec2(6.0f, 4.0f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 16.0f;
        style.GrabMinSize = 14.0f;
        break;

    default:
        throw std::runtime_error("Invalid UI density");
    }
}

} // namespace

void apply_interface_style(const interface_settings& settings) {
    ImGuiStyle style;

    apply_theme(style, settings.theme);
    apply_density(style, settings.density);

    style.ScaleAllSizes(settings.scale);

    // Separators must retain at least one pixel of thickness.
    style.SeparatorSize = std::max(style.SeparatorSize, 1.0f);

    ImGui::GetStyle() = style;

    ImGui::GetIO().FontGlobalScale =
        base_font_scale * settings.scale;
}

} // namespace cas  