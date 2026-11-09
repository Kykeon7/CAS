#include <automata/rules/spiral.h>

namespace cas::automata::rules {

automata::cell_state spiral::next(
    const automata::cell_stats& stats
) const {
    return _table[stats.state_2][stats.state_1];
}

int spiral::state_count() const noexcept {
    return 3;
}

} // namespace cas::automata::rules