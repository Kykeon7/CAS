#include "automata/rules/binary.h"

namespace cas::automata::rules {

binary::binary(
    std::array<bool, 9> birth,
    std::array<bool, 9> survive
)
    : _birth(birth)
    , _survive(survive)
{
}

automata::cell_state binary::next(
    const automata::cell_stats& stats
) const {
    int alive_neighbors =
        stats.state_1 - (stats.center == 1);

    if (stats.center == 1)
        return _survive[alive_neighbors] ? 1 : 0;

    return _birth[alive_neighbors] ? 1 : 0;
}

int binary::state_count() const noexcept {
    return 2;
}

std::array<bool, 9>& binary::birth() noexcept {
    return _birth;
}

const std::array<bool, 9>& binary::birth() const noexcept {
    return _birth;
}

std::array<bool, 9>& binary::survive() noexcept {
    return _survive;
}

const std::array<bool, 9>& binary::survive() const noexcept {
    return _survive;
}

} // namespace cas::automata::rules