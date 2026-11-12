#include <gui/panels/stats.h>

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>

#include <application/settings.h>
#include <automata/rules/rule.h>
#include <gui/color_conv.h>
#include <platform/gpu_device.h>
#include <rendering/context.h>
#include <system/gpu_memory.h>
#include <system/memory.h>
#include <system/profiler.h>
#include <system/time.h>

namespace {

template <typename TUsed, typename TBudget>
ImVec4 get_memory_color(TUsed used, TBudget budget) {
    if (budget == 0)
        return cas::gui::to_imgui(cas::graphics::white);

    const double usage =
        static_cast<double>(used) / static_cast<double>(budget);

    return cas::gui::to_imgui(
        usage >= 0.90 ? cas::graphics::red :
        usage >= 0.70 ? cas::graphics::yellow :
        cas::graphics::green
    );
}

template <typename TUsed, typename TBudget>
void draw_memory_bar(TUsed used, TBudget budget) {
    const double usage =
        budget > 0
            ? static_cast<double>(used) / static_cast<double>(budget)
            : 0.0;

    const float progress = static_cast<float>(
        std::clamp(usage, 0.0, 1.0)
    );

    ImGui::PushStyleColor(
        ImGuiCol_PlotHistogram,
        get_memory_color(used, budget)
    );

    ImGui::ProgressBar(
        progress,
        ImVec2(-1.0f, 6.0f),
        ""
    );

    ImGui::PopStyleColor();
}

/// @brief Returns the inverse of a color.
[[nodiscard]]
cas::graphics::color inverse_color(const cas::graphics::color& color) {
    return {
        1.f - color.r,
        1.f - color.g,
        1.f - color.b,
        1.f
    };
}

/// @brief Converts an sRGB channel to linear RGB.
[[nodiscard]]
double linearize_channel(float channel) {
    return channel <= 0.04045f
        ? channel / 12.92
        : std::pow((channel + 0.055) / 1.055, 2.4);
}

/// @brief Calculates the relative luminance of a color.
[[nodiscard]]
double get_relative_luminance(const cas::graphics::color& color) {
    return
        0.2126 * linearize_channel(color.r) +
        0.7152 * linearize_channel(color.g) +
        0.0722 * linearize_channel(color.b);
}

/// @brief Returns a background color with sufficient contrast against the given color.
[[nodiscard]]
cas::graphics::color get_contrasting_background(
    const cas::graphics::color& foreground
) {
    const auto inverse = inverse_color(foreground);

    const double foreground_luminance =
        get_relative_luminance(foreground);

    const double inverse_luminance =
        get_relative_luminance(inverse);

    const double lighter = std::max(
        foreground_luminance,
        inverse_luminance
    );

    const double darker = std::min(
        foreground_luminance,
        inverse_luminance
    );

    if ((lighter + 0.05) / (darker + 0.05) >= 4.5)
        return inverse;

    const cas::graphics::color black{ 0.f, 0.f, 0.f };
    const cas::graphics::color white{ 1.f, 1.f, 1.f };

    const double black_contrast =
        (foreground_luminance + 0.05) / 0.05;

    const double white_contrast =
        1.05 / (foreground_luminance + 0.05);

    return black_contrast >= white_contrast ? black : white;
}

/// @brief Draws a state counter with a contrasting background.
void draw_state_count(
    int count,
    const cas::graphics::color& state_color
) {
    const std::string text = std::to_string(count);

    constexpr ImVec2 padding{ 3.0f, 1.0f };

    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str());

    const ImVec2 size{
        text_size.x + padding.x * 2.0f,
        text_size.y + padding.y * 2.0f
    };

    const ImVec2 position = ImGui::GetCursorScreenPos();

    const ImVec2 text_position{
        position.x + padding.x,
        position.y + padding.y
    };

    const ImVec2 background_end{
        position.x + size.x,
        position.y + size.y
    };

    const auto background_color =
        get_contrasting_background(state_color);

    auto* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        position,
        background_end,
        ImGui::ColorConvertFloat4ToU32(
            cas::gui::to_imgui(background_color)
        )
    );

    draw_list->AddText(
        text_position,
        ImGui::ColorConvertFloat4ToU32(
            cas::gui::to_imgui(state_color)
        ),
        text.c_str()
    );

    ImGui::Dummy(size);
}

/// @brief Begins a two-column statistics table.
[[nodiscard]]
bool begin_stats_table(const char* id) {
    if (!ImGui::BeginTable(
        id,
        2,
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_NoSavedSettings
    ))
        return false;

    ImGui::TableSetupColumn(
        "##label",
        ImGuiTableColumnFlags_WidthStretch,
        0.55f
    );

    ImGui::TableSetupColumn(
        "##value",
        ImGuiTableColumnFlags_WidthStretch,
        0.45f
    );

    return true;
}

/// @brief Begins a statistics row and positions the cursor in its value column.
void draw_stat_label(
    const char* label,
    const ImVec4& color,
    bool indented = false
) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (indented)
        ImGui::Indent(12.0f);

    ImGui::TextColored(color, "%s", label);

    if (indented)
        ImGui::Unindent(12.0f);

    ImGui::TableSetColumnIndex(1);
}

/// @brief Draws GPU diagnostics when the required information is available.
void draw_gpu_stats(
    cas::rendering::context& rendering_context,
    const ImVec4& label_color,
    const ImVec4& value_color
) {
    if (!rendering_context.is_gpu_backend()) {
        ImGui::Text("GPU backend not active.");
        return;
    }

    const auto* device = rendering_context.gpu_device();

    if (!device) {
        ImGui::Text("GPU device unavailable.");
        return;
    }

    const char* device_name = device->name();

    if (!device_name) {
        ImGui::Text("GPU device name unavailable.");
        return;
    }

    const auto memory = cas::system::get_gpu_memory_info(device_name);

    if (!memory.available) {
        ImGui::Text("GPU memory information unavailable.");
        return;
    }

    if (!begin_stats_table("##gpu"))
        return;

    constexpr double bytes_to_gib =
        1.0 / (1024.0 * 1024.0 * 1024.0);

    draw_stat_label("Device:", label_color);
    ImGui::TextColored(value_color, "%s", device_name);

    draw_stat_label("Dedicated Capacity:", label_color);
    ImGui::TextColored(
        value_color,
        "%.2f GiB",
        memory.dedicated_capacity_bytes * bytes_to_gib
    );

    draw_stat_label("Shared Capacity:", label_color);
    ImGui::TextColored(
        value_color,
        "%.2f GiB",
        memory.shared_capacity_bytes * bytes_to_gib
    );

    draw_stat_label("Local Usage:", label_color);
    ImGui::TextColored(
        get_memory_color(
            memory.local_used_bytes,
            memory.local_budget_bytes
        ),
        "%.2f GiB",
        memory.local_used_bytes * bytes_to_gib
    );

    draw_memory_bar(
        memory.local_used_bytes,
        memory.local_budget_bytes
    );

    draw_stat_label("Local Budget:", label_color);
    ImGui::TextColored(
        value_color,
        "%.2f GiB",
        memory.local_budget_bytes * bytes_to_gib
    );

    draw_stat_label("Non-local Usage:", label_color);
    ImGui::TextColored(
        get_memory_color(
            memory.non_local_used_bytes,
            memory.non_local_budget_bytes
        ),
        "%.2f GiB",
        memory.non_local_used_bytes * bytes_to_gib
    );

    draw_memory_bar(
        memory.non_local_used_bytes,
        memory.non_local_budget_bytes
    );

    draw_stat_label("Non-local Budget:", label_color);
    ImGui::TextColored(
        value_color,
        "%.2f GiB",
        memory.non_local_budget_bytes * bytes_to_gib
    );

    ImGui::EndTable();
}

/// @brief Draws a population density bar.
void draw_population_density_bar(double density) {
    const float progress = static_cast<float>(
        std::clamp(density, 0.0, 1.0)
        );

    const auto color = cas::gui::to_imgui(
        density >= 0.90 ? cas::graphics::red :
        density >= 0.70 ? cas::graphics::yellow :
        cas::graphics::green
    );

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);

    ImGui::ProgressBar(
        progress,
        ImVec2(-1.0f, 6.0f),
        ""
    );

    ImGui::PopStyleColor();
}

} // namespace
namespace cas::gui::panels {

stats::stats(
    viewport_view& view,
    const stats_panel_settings& settings,
    rendering::context& rendering_context
) :
    _view(view),
    _settings(settings),
    _rendering_context(rendering_context)
{
}

void stats::render() {
    ImGui::Text("Stats");
    ImGui::Separator();

    const auto label_color = to_imgui(graphics::gray);
    const auto value_color = to_imgui(graphics::white);

    if (_settings.show_performance) {
        auto& profiler = system::profiler::instance();

        ImGui::Text("Performance");

        if (begin_stats_table("##performance")) {
            const double delta_time =
                time::instance().unscaled_delta_time();

            const double fps =
                delta_time > 0.0 ? 1.0 / delta_time : 0.0;

            const auto fps_color = to_imgui(
                fps <= 20.0 ? graphics::red :
                fps < 60.0 ? graphics::yellow :
                graphics::green
            );

            draw_stat_label("FPS:", label_color);
            ImGui::TextColored(fps_color, "%.1f", fps);

            draw_stat_label("Frame Time:", label_color);
            ImGui::TextColored(
                value_color,
                "%.2f ms",
                profiler.get_ms(system::profile_id::run)
            );

            draw_stat_label("Simulation Time:", label_color, true);
            ImGui::TextColored(
                value_color,
                "%.2f ms",
                profiler.get_ms(system::profile_id::update)
            );

            draw_stat_label("Render Time:", label_color, true);
            ImGui::TextColored(
                value_color,
                "%.2f ms",
                profiler.get_ms(system::profile_id::render)
            );

            ImGui::EndTable();
        }

        ImGui::Separator();
    }

    if (_settings.show_memory) {
        ImGui::Text("Memory");

        if (begin_stats_table("##memory")) {
            draw_stat_label("Process Memory:", label_color);
            ImGui::TextColored(
                value_color,
                "%.2f MB",
                system::get_memory_usage_mb()
            );

            draw_stat_label("Private Memory:", label_color);
            ImGui::TextColored(
                value_color,
                "%.2f MB",
                system::get_private_usage_mb()
            );

            ImGui::EndTable();
        }

        ImGui::Separator();
    }

    if (_settings.show_simulation) {
        const auto& automaton = _view.simulation().automaton();
        const auto& config = _view.config();

        ImGui::Text("Simulation");

        if (begin_stats_table("##simulation")) {
            draw_stat_label("Generation:", label_color);
            ImGui::TextColored(
                value_color,
                "%d",
                automaton.generation_count()
            );

            std::int64_t total_cells = 0;
            std::int64_t active_cells = 0;

            for (int state = 0; state < automaton.rule().state_count(); ++state) {
                const auto state_count = automaton.state_count(state);

                total_cells += state_count;

                if (state != 0)
                    active_cells += state_count;
            }

            draw_stat_label("Total Cells:", label_color);
            ImGui::TextColored(
                value_color,
                "%lld",
                static_cast<long long>(total_cells)
            );

            for (int state = 0; state < automaton.rule().state_count(); ++state) {
                const auto state_color =
                    state == 0 ? config.background_color :
                    state == 1 ? config.alive_color :
                    config.persistent_color;

                char label[32];
                std::snprintf(
                    label,
                    sizeof(label),
                    "State %d Cells:",
                    state
                );

                draw_stat_label(label, label_color);

                draw_state_count(
                    automaton.state_count(state),
                    state_color
                );
            }

            draw_stat_label("Active Cells:", label_color);
            ImGui::TextColored(
                value_color,
                "%lld",
                static_cast<long long>(active_cells)
            );

            const double population_density =
                total_cells > 0
                ? static_cast<double>(active_cells) /
                static_cast<double>(total_cells)
                : 0.0;

            draw_stat_label("Population Density:", label_color);
            ImGui::TextColored(
                get_memory_color(active_cells, total_cells),
                "%.1f%%",
                population_density * 100.0
            );

            ImGui::EndTable();

            draw_population_density_bar(population_density);
        }

        ImGui::Separator();
    }

    if (_settings.show_gpu) {
        ImGui::Text("GPU");

        draw_gpu_stats(
            _rendering_context,
            label_color,
            value_color
        );

        ImGui::Separator();
    }
}

} // namespace cas::gui::panels