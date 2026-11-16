#include <automata/experiment_presets.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <automata/automaton.h>
#include <automata/pattern_tests.h>
#include <automata/patterns.h>
#include <automata/rules/factory.h>
#include <automata/topologies.h>

namespace cas::automata::experiments {

namespace {

void place(automaton& target, const pattern& source, int x, int y) {
    if (!stamp_pattern(target, source, x, y))
        throw std::runtime_error("Experiment pattern does not fit the target automaton.");
}

} // namespace

automaton create(id identifier) {
    const auto index = static_cast<std::size_t>(identifier);
    if (index >= catalogue.size())
        throw std::invalid_argument("Unknown experiment identifier.");

    const auto& entry = catalogue[index];
    auto rule = rules::string_to_rule(std::string(entry.rule));
    if (!rule)
        throw std::runtime_error("Experiment rule is unavailable.");

    auto topology = entry.hexagonal ? topologies::hex_finite() : topologies::moore_finite();
    topology.set_boundary(entry.boundary);
    automaton result(entry.width, entry.height, std::move(topology), std::move(rule));

    switch (identifier) {
    case id::gosper_gun:
        place(result, patterns::create(patterns::id::gosper_glider_gun), 44, 35);
        break;
    case id::glider_collision: {
        const auto glider = patterns::create(patterns::id::glider);
        place(result, glider, 30, 30);
        // A second glider mirrored across the horizontal axis.
        std::vector<pattern_cell> reflected;
        for (const auto& cell : glider.cells())
            reflected.push_back({{-cell.offset.x, -cell.offset.y}, cell.state});
        place(result, pattern(std::move(reflected)), 44, 44);
        break;
    }
    case id::highlife_replicator:
        place(result, patterns::create(patterns::id::highlife_replicator), 60, 60);
        break;
    case id::spiral_single:
        result.set(48, 48, 1);
        break;
    case id::spiral_four:
        for (int x = 46; x < 50; ++x)
            result.set(x, 48, 1);
        break;
    case id::spiral_eight:
        for (int x = 60; x < 68; ++x)
            result.set(x, 64, 1);
        break;
    case id::spiral_multistate:
        place(result, patterns::create(patterns::id::spiral_cluster), 46, 46);
        break;
    }
    return result;
}

} // namespace cas::automata::experiments
