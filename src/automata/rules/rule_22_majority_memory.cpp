
#include "automata/rules/rule_22_majority_memory.h"

namespace cas::automata::rules {

cell_state rule_22_majority_memory::next(
    const cell_stats& stats
) const {
    return stats.state_1 == 1 ? 1 : 0;
}

int rule_22_majority_memory::state_count() const noexcept {
    return 2;
}

int rule_22_majority_memory::memory_depth() const noexcept {
    return 4;
}

} // namespace cas::automata::rules
