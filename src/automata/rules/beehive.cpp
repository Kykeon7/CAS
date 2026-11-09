#include <automata/rules/beehive.h>

namespace cas::automata::rules {

    automata::cell_state beehive::next(
        const automata::cell_stats& stats
    ) const {
        int state_2 =
            stats.state_2 - (stats.center == 2);

        int state_1 =
            stats.state_1 - (stats.center == 1);

        int index = 0;

        for (int i = 0; i < state_2; ++i)
            index += 7 - i;

        index += state_1;

        return _table[index];
    }

    int beehive::state_count() const noexcept {
        return 3;
    }

} // namespace cas::automata::rules