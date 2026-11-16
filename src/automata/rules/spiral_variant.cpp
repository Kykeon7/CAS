#include <automata/rules/spiral_variant.h>

#include <stdexcept>

namespace cas::automata::rules {

automata::cell_state spiral_variant::next(
    const automata::cell_stats& stats
) const {
    const int total =
        stats.state_0 +
        stats.state_1 +
        stats.state_2;

    if (stats.center > 2 ||
        stats.state_0 < 0 ||
        stats.state_1 < 0 ||
        stats.state_2 < 0 ||
        total < 1 ||
        total > 7 ||
        stats.state_1 > 7 ||
        stats.state_2 > 7)
    {
        throw std::invalid_argument(
            "Invalid cell statistics for the Spiral rule."
        );
    }

    return _table[stats.state_2][stats.state_1];
}

int spiral_variant::state_count() const noexcept {
    return 3;
}

} // namespace cas::automata::rules