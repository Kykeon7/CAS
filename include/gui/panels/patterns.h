#pragma once

#include <vector>
#include <string>

#include <automata/pattern_tests.h>
#include <automata/experiment_presets.h>
#include <gui/panels/panel.h>
#include <gui/viewport_view.h>

namespace cas::gui::panels {

/// @brief Presents compatible pattern presets and deterministic regression cases.
class patterns final : public panel {
public:
    explicit patterns(viewport_view& view);
    patterns(const patterns&) = delete;
    patterns& operator=(const patterns&) = delete;
    patterns(patterns&&) = delete;
    patterns& operator=(patterns&&) = delete;
    ~patterns() override = default;

    void render() override;

private:
    void render_experiments();

    viewport_view& _view;
    int _pending_experiment = -1;
    std::string _experiment_error;
    std::vector<automata::pattern_test_result> _results;
};

} // namespace cas::gui::panels
