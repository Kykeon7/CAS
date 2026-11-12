#include <gui/panels/stats.h>

#include <imgui.h>

#include <application/settings.h>
#include <automata/rules/rule.h>
#include <platform/gpu_device.h>
#include <rendering/context.h>
#include <system/gpu_memory.h>
#include <system/memory.h>
#include <system/profiler.h>
#include <system/time.h>

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

    if (_settings.show_performance) {
        auto& profiler = system::profiler::instance();

        ImGui::Text("Performance");

        const double delta_time = time::instance().delta_time();

        ImGui::Text(
            "FPS: %.1f",
            delta_time > 0.0 ? 1.0 / delta_time : 0.0
        );

        ImGui::Text(
            "Frame Time: %.2f ms",
            profiler.get_ms(system::profile_id::run)
        );

        ImGui::Text(
            "Simulation Time: %.2f ms",
            profiler.get_ms(system::profile_id::update)
        );

        ImGui::Text(
            "Render Time: %.2f ms",
            profiler.get_ms(system::profile_id::render)
        );

        ImGui::Separator();
    }

    if (_settings.show_memory) {
        ImGui::Text("Memory");

        ImGui::Text(
            "Process Memory: %.2f MB",
            system::get_memory_usage_mb()
        );

        ImGui::Text(
            "Private Memory: %.2f MB",
            system::get_private_usage_mb()
        );

        ImGui::Separator();
    }

    if (_settings.show_simulation) {
        const auto& automaton = _view.simulation().automaton();

        ImGui::Text("Simulation");

        ImGui::Text(
            "Generation: %d",
            automaton.generation_count()
        );

        ImGui::Text(
            "State 0 Cells: %d",
            automaton.state_count(0)
        );

        ImGui::Text(
            "State 1 Cells: %d",
            automaton.state_count(1)
        );

        if (automaton.rule().state_count() >= 3) {
            ImGui::Text(
                "State 2 Cells: %d",
                automaton.state_count(2)
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

        ImGui::Text("Device: %s", device_name);

        const auto memory = system::get_gpu_memory_info(device_name);

        if (!memory.available) {
            ImGui::Text("GPU memory information unavailable.");
            ImGui::Separator();
            return;
        }

        constexpr double bytes_to_gib =
            1.0 / (1024.0 * 1024.0 * 1024.0);

        ImGui::Separator();

        ImGui::Text(
            "Dedicated Memory Capacity: %.2f GiB",
            memory.dedicated_capacity_bytes * bytes_to_gib
        );

        ImGui::Text(
            "Shared Memory Capacity: %.2f GiB",
            memory.shared_capacity_bytes * bytes_to_gib
        );

        ImGui::Separator();

        ImGui::Text(
            "Local Memory Usage: %.2f GiB",
            memory.local_used_bytes * bytes_to_gib
        );

        ImGui::Text(
            "Local Memory Budget: %.2f GiB",
            memory.local_budget_bytes * bytes_to_gib
        );

        ImGui::Text(
            "Non-local Memory Usage: %.2f GiB",
            memory.non_local_used_bytes * bytes_to_gib
        );

        ImGui::Text(
            "Non-local Memory Budget: %.2f GiB",
            memory.non_local_budget_bytes * bytes_to_gib
        );

        ImGui::Separator();
    }
}

} // namespace cas::gui::panels