#pragma once

#include <automata/automaton.h>

#include <files/serialization/progress.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <vector>

namespace cas::files::serialization {

struct automaton_snapshot {
    int width = 0;
    int height = 0;
    int generation = 0;

    // These contain only the small rule/topology metadata,
    // not the potentially large cell array.
    nlohmann::json rule;
    nlohmann::json topology;

    std::vector<automata::cell_state> cells;
};

[[nodiscard]]
automaton_snapshot capture_automaton(
    const automata::automaton& value
);

[[nodiscard]]
nlohmann::json save_automaton(
    const automaton_snapshot& snapshot
);

[[nodiscard]]
nlohmann::json save_automaton(
    const automata::automaton& value
);

automata::automaton load_automaton(
    const nlohmann::json& document,
    progress_reporter progress = {}
);

automata::automaton load_automaton_file(
    const std::filesystem::path& path,
    progress_callback on_progress = {}
);

void save_automaton_file(
    const std::filesystem::path& path,
    const automata::automaton& value
);

void save_automaton_file(
    const std::filesystem::path& path,
    const automaton_snapshot& snapshot
);

} // namespace cas::files::serialization
