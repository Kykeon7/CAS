#include "topology.h"

namespace cas::topologies {

    inline topology HexFinite() {
        return topology({
            { 1, 0}, {-1, 0},
            { 0, 1}, { 0,-1},
            { 1,-1}, {-1, 1}
            }, boundary_type::Finite);
    }

} // namespace cas::topologies