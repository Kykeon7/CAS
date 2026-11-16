#include <automata/pattern_tests.h>

#include <algorithm>
#include <exception>
#include <utility>
#include <vector>

#include <automata/automaton.h>
#include <automata/rules/factory.h>
#include <automata/rules/rule.h>
#include <automata/topologies.h>

namespace cas::automata {

bool stamp_pattern(automaton& target, const pattern& source, int x, int y) {
    for (const auto& cell : source.cells()) {
        if (cell.state == 0 || cell.state >= target.rule().state_count() ||
            !target.in_bounds(x + cell.offset.x, y + cell.offset.y))
            return false;
    }

    for (const auto& cell : source.cells())
        target.set(x + cell.offset.x, y + cell.offset.y, cell.state);

    return true;
}

namespace {

bool equals_pattern(const automaton& a, const pattern& expected, int x, int y) {
    if (a.state_count(1) != static_cast<int>(expected.cells().size()))
        return false;

    for (const auto& cell : expected.cells()) {
        if (a.get(x + cell.offset.x, y + cell.offset.y) != cell.state)
            return false;
    }
    return true;
}

pattern_test_result test_pattern(patterns::id id, int steps, int dx, int dy) {
    const auto& descriptor = patterns::catalogue[static_cast<std::size_t>(id)];
    pattern_test_result result{ std::string(descriptor.name), false, {} };
    try {
        automaton a(32, 32, topologies::moore_finite(), rules::game_of_life());
        const auto source = patterns::create(id);
        constexpr int x = 12;
        constexpr int y = 12;
        if (!stamp_pattern(a, source, x, y)) {
            result.detail = "Could not initialize pattern";
            return result;
        }
        for (int i = 0; i < steps; ++i)
            a.step();
        result.passed = equals_pattern(a, source, x + dx, y + dy);
        result.detail = result.passed ? "Expected configuration verified" :
            "Unexpected cell configuration";
    }
    catch (const std::exception& e) {
        result.detail = e.what();
    }
    return result;
}

pattern_test_result test_multistate_stamp() {
    pattern_test_result result{"Multistate placement", false, {}};
    try {
        automaton a(32, 32, topologies::moore_finite(), rules::game_of_life());
        const pattern source(std::vector<pattern_cell>{
            {{0, 0}, 1}, {{1, 0}, 2}
        });
        // A binary automaton must reject the whole pattern atomically.
        result.passed = !stamp_pattern(a, source, 12, 12) &&
            a.get(12, 12) == 0 && a.get(13, 12) == 0;
        result.detail = result.passed ? "Invalid state rejected atomically" :
            "Invalid state modified the automaton";
    }
    catch (const std::exception& e) {
        result.detail = e.what();
    }
    return result;
}

} // namespace

std::vector<pattern_test_result> run_pattern_tests() {
    return {
        test_pattern(patterns::id::block, 1, 0, 0),
        test_pattern(patterns::id::blinker, 2, 0, 0),
        test_pattern(patterns::id::glider, 4, 1, 1),
        test_multistate_stamp()
    };
}

} // namespace cas::automata
