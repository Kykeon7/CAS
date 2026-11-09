
#pragma once

#include "automata/cell_state.h"

#include <span>
#include <automata/cell_stats.h>

namespace cas::automata::rules {

class rule {
public:
    virtual ~rule() = default;

    virtual automata::cell_state next(
        const automata::cell_stats& stats
    ) const = 0;

    virtual int state_count() const noexcept = 0;

    // Number of previous generations required by the rule.
    // Zero means no temporal memory.
    virtual int memory_depth() const noexcept {
        return 0;
    }
};

} // namespace cas::automata::rules
