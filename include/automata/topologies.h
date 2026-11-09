
#pragma once

#include "topology.h"

namespace cas::automata::topologies {

inline topology hex_finite() {
    return topology({
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1},
        { 1, -1},
        {-1,  1}
    }, boundary_type::finite);
}

inline topology linear_finite() {
    return topology({
        {-1, 0},
        { 1, 0}
    }, boundary_type::finite);
}

inline topology linear_toroidal() {
    return topology({
        {-1, 0},
        { 1, 0}
    }, boundary_type::toroidal);
}

} // namespace cas::automata::topologies
