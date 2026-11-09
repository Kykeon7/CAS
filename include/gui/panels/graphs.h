#pragma once

#include <gui/panels/panel.h>
#include <gui/viewport_view.h>

#include <span>
#include <cstdint>

namespace cas::gui::panels {

enum class graph_scale {
    linear,
    log10,
    square_root
};

enum class graph_range {
    auto_fit,
    zero_based
};

struct graph_config {
    graph_scale scale = graph_scale::linear;
    graph_range range = graph_range::auto_fit;
    bool show_grid = true;
    bool show_statistics = true;
    bool follow_latest = true;
};

class graphs final : public panel {
public:
    explicit graphs(viewport_view& view);

    void render() override;

private:
    void render_population();
    void render_activity();
    void render_density();

    void render_graph(
        const char* id,
        const char* label,
        std::span<const float> history,
        std::uint64_t total_samples,
        graph_config& config
    );

    void render_graph_controls(graph_config& config);
    void render_statistics(std::span<const float> history);

    viewport_view& _view;

    graph_config _population_config;
    graph_config _activity_config;
    graph_config _density_config;
};

} // namespace cas::gui::panels