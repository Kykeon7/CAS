#pragma once

#include <math/vec2.h>

#include <vector>

namespace cas::automata {

class pattern {
public:
    pattern() = default;

    pattern(std::vector<math::vec2i> offsets);

    const std::vector<math::vec2i>& offsets() const noexcept;

private:
    std::vector<math::vec2i> _offsets;
};

} // namespace cas::automata