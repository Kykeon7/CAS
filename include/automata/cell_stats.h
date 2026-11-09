#pragma once
#include "cell_state.h"

namespace cas::automata {

struct cell_stats {
    cell_state center{ 0 };

    int state_0{ 0 };
    int state_1{ 0 };
    int state_2{ 0 };
};

} // namespace cas::automata