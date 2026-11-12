#include <gui/panels/stats.h>

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
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

        const double delta_time =
            time::instance().unscaled_delta_time();

        const double fps =
            delta_time > 0.0 ? 1.0 / delta_time : 0.0;

        const auto fps_color = to_imgui(
            fps <= 20.0 ? graphics::red :
            fps < 60.0 ? graphics::yellow :
            graphics::green
        );

        ImGui::TextColored(label_color, "FPS:");
        ImGui::SameLine();
        ImGui::TextColored(fps_color, "%.1f", fps);

        ImGui::TextColored(label_color, "Frame Time:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f ms",
            profiler.get_ms(system::profile_id::run)
        );

        ImGui::Indent(12.0f);

        ImGui::TextColored(label_color, "Simulation Time:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f ms",
            profiler.get_ms(system::profile_id::update)
        );

        ImGui::TextColored(label_color, "Render Time:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f ms",
            profiler.get_ms(system::profile_id::render)
        );

        ImGui::Unindent(12.0f);

        ImGui::Separator();
    }

    if (_settings.show_memory) {
        ImGui::Text("Memory");

        ImGui::TextColored(label_color, "Process Memory:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f MB",
            system::get_memory_usage_mb()
        );

        ImGui::TextColored(label_color, "Private Memory:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f MB",
            system::get_private_usage_mb()
        );

        ImGui::Separator();
    }

    if (_settings.show_simulation) {
        const auto& automaton = _view.simulation().automaton();
        const auto& config = _view.config();

        ImGui::Text("Simulation");

        ImGui::TextColored(label_color, "Generation:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%d",
            automaton.generation_count()
        );

        std::int64_t total_cells = 0;

        for (int state = 0; state < automaton.rule().state_count(); ++state)
            total_cells += automaton.state_count(state);

        ImGui::TextColored(label_color, "Total Cells:");
        ImGui::SameLine();
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

            ImGui::TextColored(
                label_color,
                "State %d Cells:",
                state
            );

            ImGui::SameLine();

            draw_state_count(
                automaton.state_count(state),
                state_color
            );
        }

        ImGui::Separator();
    }

    if (_settings.show_gpu) {
        ImGui::Text("GPU");

        if (!_rendering_context.is_gpu_backend()) {
            ImGui::Text("GPU backend not active.");
            ImGui::Separator();
            return;
        }

        const auto* device = _rendering_context.gpu_device();

        if (!device) {
            ImGui::Text("GPU device unavailable.");
            ImGui::Separator();
            return;
        }

        const char* device_name = device->name();

        if (!device_name) {
            ImGui::Text("GPU device name unavailable.");
            ImGui::Separator();
            return;
        }

        ImGui::TextColored(label_color, "Device:");
        ImGui::SameLine();
        ImGui::TextColored(value_color, "%s", device_name);

        const auto memory = system::get_gpu_memory_info(device_name);

        if (!memory.available) {
            ImGui::Text("GPU memory information unavailable.");
            ImGui::Separator();
            return;
        }

        constexpr double bytes_to_gib =
            1.0 / (1024.0 * 1024.0 * 1024.0);

        ImGui::Separator();

        ImGui::TextColored(label_color, "Dedicated Memory Capacity:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f GiB",
            memory.dedicated_capacity_bytes * bytes_to_gib
        );

        ImGui::TextColored(label_color, "Shared Memory Capacity:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f GiB",
            memory.shared_capacity_bytes * bytes_to_gib
        );

        ImGui::Separator();

        ImGui::TextColored(label_color, "Local Memory Usage:");
        ImGui::SameLine();
        ImGui::TextColored(
            get_memory_color(
                memory.local_used_bytes,
                memory.local_budget_bytes
            ),
            "%.2f GiB",
            memory.local_used_bytes* bytes_to_gib
        );

        draw_memory_bar(
            memory.local_used_bytes,
            memory.local_budget_bytes
        );

        ImGui::TextColored(label_color, "Local Memory Budget:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f GiB",
            memory.local_budget_bytes* bytes_to_gib
        );

        ImGui::TextColored(label_color, "Non-local Memory Usage:");
        ImGui::SameLine();
        ImGui::TextColored(
            get_memory_color(
                memory.non_local_used_bytes,
                memory.non_local_budget_bytes
            ),
            "%.2f GiB",
            memory.non_local_used_bytes* bytes_to_gib
        );

        draw_memory_bar(
            memory.non_local_used_bytes,
            memory.non_local_budget_bytes
        );

        ImGui::TextColored(label_color, "Non-local Memory Budget:");
        ImGui::SameLine();
        ImGui::TextColored(
            value_color,
            "%.2f GiB",
            memory.non_local_budget_bytes* bytes_to_gib
        );

        ImGui::Separator();
    }
}

} // namespace cas::gui::panels