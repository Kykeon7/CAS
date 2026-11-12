#include <automata/rules/beehive.h>

#include <cstddef>
#include <stdexcept>

namespace cas::automata::rules {

automata::cell_state beehive::next(
    const automata::cell_stats& stats
) const {
    int state_0 =
        stats.state_0 - (stats.center == 0);

    int state_1 =
        stats.state_1 - (stats.center == 1);

    int state_2 =
        stats.state_2 - (stats.center == 2);

    const int total =
        state_0 + state_1 + state_2;

    if (stats.center > 2 ||
        state_0 < 0 ||
        state_1 < 0 ||
        state_2 < 0 ||
        total > 6)
    {
        throw std::invalid_argument(
            "Invalid cell statistics for the Beehive rule."
        );
    }

    int index = 0;

    for (int i = 0; i < state_2; ++i)
        index += 7 - i;

    index += state_1;

    if (index < 0 ||
        index >= static_cast<int>(_table.size()))
    {
        throw std::out_of_range(
            "Beehive rule table index is out of range."
        );
    }

    return _table[static_cast<std::size_t>(index)];
}

int beehive::state_count() const noexcept {
    return 3;
}

} // namespace cas::automata::rules