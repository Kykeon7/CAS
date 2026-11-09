#pragma once

#include "rule.h"

#include <array>
#include <automata/cell_stats.h>

namespace cas::automata::rules {

class beehive : public rule {
public:
    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    int state_count() const noexcept override;

private:
    static constexpr std::array<automata::cell_state, 28> _table{ {
        0, 1, 2, 1, 2, 0, 0,
        0, 2, 2, 2, 1, 1,
        0, 0, 2, 2, 0,
        0, 2, 2, 0,
        0, 0, 2,
        2, 0, 0
    } };
};

} // namespace cas::automata::rules