
#pragma once

#include "automata/rules/rule.h"

namespace cas::automata::rules {

class rule_22_majority_memory final : public rule {
public:
    cell_state next(
        const cell_stats& stats
    ) const override;

    int state_count() const noexcept override;

    int memory_depth() const noexcept override;
};

} // namespace cas::automata::rules
