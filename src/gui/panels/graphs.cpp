#include <gui/panels/graphs.h>

#include <imgui.h>
#include <implot.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

namespace cas::gui::panels {

graphs::graphs(viewport_view& view)
    : _view(view)
{}

void graphs::render() {
    ImGui::Text("Graphs");

    if (ImGui::BeginTabBar("##GraphsTabs")) {
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
}

void graphs::render_population() {
    const auto& history =
        _view.get().population_history();

    render_graph(
        "##PopulationPlot",
        "Population",
        _view.population_history(),
        history.total_samples(),
        _population_config
    );
}

void graphs::render_activity() {
    const auto& history =
        _view.get().activity_history();

    render_graph(
        "##ActivityPlot",
        "Population Change",
        _view.activity_history(),
        history.total_samples(),
        _activity_config
    );
}

void graphs::render_density() {
    const auto& history =
        _view.get().density_history();

    render_graph(
        "##DensityPlot",
        "Density",
        _view.density_history(),
        history.total_samples(),
        _density_config
    );
}

void graphs::render_graph_controls(graph_config& config) {
    const char* scale_items[] = {
        "Linear",
        "Log10",
        "Square root"
    };

    int scale = static_cast<int>(config.scale);

    ImGui::SetNextItemWidth(140.0f);

    if (ImGui::Combo(
        "Scale",
        &scale,
        scale_items,
        IM_ARRAYSIZE(scale_items)
    )) {
        config.scale = static_cast<graph_scale>(scale);
    }

    ImGui::SameLine();

    const char* range_items[] = {
        "Auto-fit",
        "Zero-based"
    };

    int range = static_cast<int>(config.range);

    ImGui::SetNextItemWidth(130.0f);

    if (ImGui::Combo(
        "Range",
        &range,
        range_items,
        IM_ARRAYSIZE(range_items)
    )) {
        config.range = static_cast<graph_range>(range);
    }

    ImGui::SameLine();

    ImGui::Checkbox("Grid", &config.show_grid);

    ImGui::SameLine();

    ImGui::Checkbox(
        "Statistics",
        &config.show_statistics
    );

    ImGui::SameLine();

    ImGui::Checkbox(
        "Follow latest",
        &config.follow_latest
    );
}

void graphs::render_statistics(
    std::span<const float> history
) {
    if (history.empty())
        return;

    const auto [min_it, max_it] = std::minmax_element(
        history.begin(),
        history.end()
    );

    const float average = std::accumulate(
        history.begin(),
        history.end(),
        0.0f
    ) / static_cast<float>(history.size());

    ImGui::Text(
        "Current: %.3f",
        history.back()
    );

    ImGui::SameLine();

    ImGui::Text(
        "Minimum: %.3f",
        *min_it
    );

    ImGui::SameLine();

    ImGui::Text(
        "Maximum: %.3f",
        *max_it
    );

    ImGui::SameLine();

    ImGui::Text(
        "Average: %.3f",
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

    if (history.empty()) {
        ImGui::TextDisabled("No data available.");
        return;
    }

    if (config.show_statistics)
        render_statistics(history);

    const std::uint64_t first_sample =
        total_samples - history.size();

    const double x_start =
        static_cast<double>(first_sample);

    const double x_end =
        static_cast<double>(total_samples - 1);

    const float* plot_data = history.data();

    std::vector<float> transformed_data;

    if (config.scale == graph_scale::square_root) {
        transformed_data.reserve(history.size());

        for (const float value : history) {
            transformed_data.push_back(
                std::sqrt((std::max)(value, 0.0f))
            );
        }

        plot_data = transformed_data.data();
    }
    else if (config.scale == graph_scale::log10) {
        float min_positive = 0.0f;

        for (const float value : history) {
            if (value > 0.0f &&
                (min_positive == 0.0f || value < min_positive)) {
                min_positive = value;
            }
        }

        const float zero_value = min_positive > 0.0f
            ? min_positive * 0.1f
            : 0.1f;

        transformed_data.reserve(history.size());

        for (const float value : history) {
            transformed_data.push_back(
                (std::max)(value, zero_value)
            );
        }

        plot_data = transformed_data.data();
    }

    if (ImPlot::BeginPlot(
        id,
        ImVec2(-1.0f, -1.0f),
        ImPlotFlags_NoTitle
    )) {
        ImPlotAxisFlags x_flags =
            config.show_grid
                ? ImPlotAxisFlags_None
                : ImPlotAxisFlags_NoGridLines;

        ImPlotAxisFlags y_flags = x_flags;

        if (config.range == graph_range::auto_fit) {
            y_flags |= ImPlotAxisFlags_AutoFit;
        }

        ImPlot::SetupAxis(
            ImAxis_X1,
            "Generation",
            x_flags
        );

        ImPlot::SetupAxis(
            ImAxis_X1,
            "Generation",
            x_flags
        );

        ImPlot::SetupAxis(
            ImAxis_Y1,
            label,
            y_flags
        );

        // Configurar la escala antes de establecer límites.
        if (config.scale == graph_scale::log10) {
            ImPlot::SetupAxisScale(
                ImAxis_Y1,
                ImPlotScale_Log10
            );
        }

        if (config.range == graph_range::zero_based) {
            double min_limit = 0.0;

            if (config.scale == graph_scale::log10) {
                float min_positive = 0.0f;

                for (const float value : history) {
                    if (value > 0.0f &&
                        (min_positive == 0.0f ||
                         value < min_positive)) {
                        min_positive = value;
                    }
                }

                min_limit = min_positive > 0.0f
                    ? min_positive * 0.1
                    : 0.1;
            }

            ImPlot::SetupAxisLimitsConstraints(
                ImAxis_Y1,
                min_limit,
                HUGE_VAL
            );
        }

        if (config.follow_latest) {
            ImPlot::SetupAxisLimits(
                ImAxis_X1,
                x_start,
                (std::max)(x_start + 1.0, x_end),
                ImGuiCond_Always
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
            const ImPlotPoint mouse =
                ImPlot::GetPlotMousePos();

            const auto index = static_cast<std::int64_t>(
                std::round(mouse.x - x_start)
            );

            if (index >= 0 &&
                index < static_cast<std::int64_t>(history.size())) {

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
                    history[static_cast<std::size_t>(index)]
                );

                ImGui::EndTooltip();
            }
        }

        ImPlot::EndPlot();
    }
}

} // namespace cas::gui::panels