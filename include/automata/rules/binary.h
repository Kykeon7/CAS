#pragma once

#include "rule.h"

#include <array>
#include <automata/cell_state.h>
#include <automata/cell_stats.h>

namespace cas::automata::rules {

class binary : public rule {
public:
    binary(
        std::array<bool, 9> birth,
        std::array<bool, 9> survive
    );

    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    int state_count() const noexcept override;

    std::array<bool, 9>& birth() noexcept;
    const std::array<bool, 9>& birth() const noexcept;

    std::array<bool, 9>& survive() noexcept;
    const std::array<bool, 9>& survive() const noexcept;

private:
    std::array<bool, 9> _birth;
    std::array<bool, 9> _survive;
};

} // namespace cas::automata::rules