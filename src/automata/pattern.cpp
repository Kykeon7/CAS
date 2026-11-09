#include "automata/pattern.h"

#include <utility>

namespace cas::automata {

pattern::pattern(std::vector<math::vec2i> offsets)
    : _offsets(std::move(offsets))
{
}

const std::vector<math::vec2i>& pattern::offsets() const noexcept {
    return _offsets;
}

} // namespace cas::automata