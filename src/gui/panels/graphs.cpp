
#include <gui/panels/graphs.h>

#include <imgui.h>
#include <implot.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <vector>

namespace cas::gui::panels {

graphs::graphs(viewport_view& view) :
    _view(view)
{
}

void graphs::render() {
    if (!ImGui::BeginTabBar("##GraphsTabs"))
        return;

    if (ImGui::BeginTabItem("Population")) {
        render_population();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Activity")) {
        render_activity();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Density")) {
        render_density();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
}

void graphs::render_population() {
    const auto& history = _view.get().population_history();

    render_graph(
        "##PopulationGraph",
        "Population",
        _view.population_history(),
        history.total_samples(),
        _population_config
    );
}

void graphs::render_activity() {
    const auto& history = _view.get().activity_history();

    render_graph(
        "##ActivityGraph",
        "Activity",
        _view.activity_history(),
        history.total_samples(),
        _activity_config
    );
}

void graphs::render_density() {
    const auto& history = _view.get().density_history();

    render_graph(
        "##DensityGraph",
        "Density",
        _view.density_history(),
        history.total_samples(),
        _density_config
    );
}

void graphs::render_graph_controls(graph_config& config) {
    const char* scale_names[] = {
        "Linear",
        "Log10",
        "Square root"
    };

    int scale = static_cast<int>(config.scale);

    ImGui::SetNextItemWidth(110.0f);

    if (ImGui::Combo(
        "Scale",
        &scale,
        scale_names,
        IM_ARRAYSIZE(scale_names)
    )) {
        config.scale = static_cast<graph_scale>(scale);
    }

    ImGui::SameLine();

    const char* range_names[] = {
        "Auto-fit",
        "Zero-based"
    };

    int range = static_cast<int>(config.range);

    ImGui::SetNextItemWidth(110.0f);

    if (ImGui::Combo(
        "Range",
        &range,
        range_names,
        IM_ARRAYSIZE(range_names)
    )) {
        config.range = static_cast<graph_range>(range);
    }

    ImGui::SameLine();
    ImGui::Checkbox("Grid", &config.show_grid);

    ImGui::SameLine();
    ImGui::Checkbox("Statistics", &config.show_statistics);

    ImGui::SameLine();
    ImGui::Checkbox("Follow latest", &config.follow_latest);
}

void graphs::render_statistics(std::span<const float> history) {
    if (history.empty())
        return;

    const auto [min_it, max_it] = std::minmax_element(
        history.begin(),
        history.end()
    );

    const double average = std::accumulate(
        history.begin(),
        history.end(),
        0.0
    ) / static_cast<double>(history.size());

    ImGui::Separator();

    ImGui::Text(
        "Current: %.3f  |  Min: %.3f  |  Max: %.3f  |  Average: %.3f",
        history.back(),
        *min_it,
        *max_it,
        average
    );
}


void graphs::render_graph(
    const char* id,
    const char* label,
    std::span<const float> history,
    std::uint64_t total_samples,
    graph_config& config
) {
    render_graph_controls(config);

    if (history.empty())
        return;

    if (history.size() > total_samples)
        return;

    if (history.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
        ))
        return;

    const std::uint64_t first_sample =
        total_samples - static_cast<std::uint64_t>(history.size());

    const double x_start = static_cast<double>(first_sample);
    const double x_end = static_cast<double>(total_samples - 1);

    std::vector<float> transformed_data;
    const float* plot_data = history.data();

    if (config.scale != graph_scale::linear) {
        transformed_data.reserve(history.size());

        for (const float value : history) {
            const double magnitude = std::abs(
                static_cast<double>(value)
            );

            double transformed = 0.0;

            switch (config.scale) {
            case graph_scale::linear:
                transformed = static_cast<double>(value);
                break;

            case graph_scale::log10:
                transformed = std::copysign(
                    std::log10(1.0 + magnitude),
                    static_cast<double>(value)
                );
                break;

            case graph_scale::square_root:
                transformed = std::copysign(
                    std::sqrt(magnitude),
                    static_cast<double>(value)
                );
                break;
            }

            transformed_data.push_back(
                static_cast<float>(transformed)
            );
        }

        plot_data = transformed_data.data();
    }

    const char* y_axis_label = "Value";

    switch (config.scale) {
    case graph_scale::linear:
        y_axis_label = "Value (linear)";
        break;

    case graph_scale::log10:
        y_axis_label = "Value (signed log10)";
        break;

    case graph_scale::square_root:
        y_axis_label = "Value (signed square root)";
        break;
    }

    const float statistics_height = config.show_statistics
        ? ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y
        : 0.0f;

    const float available_height =
        ImGui::GetContentRegionAvail().y;

    const float graph_height = std::max(
        100.0f,
        available_height - statistics_height
    );

    if (ImPlot::BeginPlot(id, ImVec2(-1.0f, graph_height))) {
        const ImPlotAxisFlags grid_flags = config.show_grid
            ? ImPlotAxisFlags_None
            : ImPlotAxisFlags_NoGridLines;

        ImPlot::SetupAxis(
            ImAxis_X1,
            "Generation",
            grid_flags
        );

        ImPlot::SetupAxis(
            ImAxis_Y1,
            y_axis_label,
            config.range == graph_range::auto_fit
            ? ImPlotAxisFlags_AutoFit | grid_flags
            : grid_flags
        );

        if (config.follow_latest) {
            ImPlot::SetupAxisLimits(
                ImAxis_X1,
                x_start,
                std::max(x_start + 1.0, x_end),
                ImGuiCond_Always
            );
        }

        if (config.range == graph_range::zero_based) {
            const auto min_it = std::min_element(
                plot_data,
                plot_data + history.size()
            );

            const double min_limit = std::min(
                0.0,
                static_cast<double>(*min_it)
            );

            ImPlot::SetupAxisLimitsConstraints(
                ImAxis_Y1,
                min_limit,
                std::numeric_limits<double>::max()
            );
        }

        ImPlot::PlotLine(
            label,
            plot_data,
            static_cast<int>(history.size()),
            1.0,
            x_start
        );

        if (ImPlot::IsPlotHovered()) {
            const ImPlotPoint mouse = ImPlot::GetPlotMousePos();

            const double sample_position = mouse.x - x_start;

            if (std::isfinite(sample_position) &&
                sample_position >= -0.5 &&
                sample_position < static_cast<double>(history.size()) - 0.5) {

                const auto index = static_cast<std::size_t>(
                    std::round(sample_position)
                    );

                ImGui::BeginTooltip();

                ImGui::Text(
                    "Generation: %llu",
                    static_cast<unsigned long long>(
                        first_sample + index
                        )
                );

                ImGui::Text(
                    "%s: %.3f",
                    label,
                    history[index]
                );

                ImGui::EndTooltip();
            }
        }

        ImPlot::EndPlot();
    }

    if (config.show_statistics)
        render_statistics(history);
}

} // namespace cas::gui::panels
