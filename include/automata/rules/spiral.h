#pragma once

#include "rule.h"

#include <array>
#include <automata/cell_state.h>
#include <automata/cell_stats.h>

namespace cas::automata::rules {

class spiral : public rule {
public:
    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    int state_count() const noexcept override;

private:
    static constexpr std::array<std::array<automata::cell_state, 8>, 8> _table{ {
            // state_2 = 0
            {{ 0, 1, 2, 1, 2, 2, 2, 0 }},

            // state_2 = 1
            {{ 0, 2, 2, 1, 2, 2, 2, 0 }},

            // state_2 = 2
            {{ 0, 0, 2, 2, 1, 2, 0, 0 }},

            // state_2 = 3
            {{ 0, 2, 2, 1, 2, 0, 0, 0 }},

            // state_2 = 4
            {{ 0, 0, 2, 1, 0, 0, 0, 0 }},

            // state_2 = 5
            {{ 0, 0, 2, 0, 0, 0, 0, 0 }},

            // state_2 = 6
            {{ 0, 0, 0, 0, 0, 0, 0, 0 }},

            // state_2 = 7
            {{ 0, 0, 0, 0, 0, 0, 0, 0 }}
        }
    };
};

} // namespace cas::automata::rules