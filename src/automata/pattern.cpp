#include <automata/pattern.h>

#include <utility>

namespace cas::automata {

pattern::pattern(std::vector<math::vec2i> offsets) {
    _cells.reserve(offsets.size());
    for (const auto& offset : offsets)
        _cells.push_back({offset, 1});
}

pattern::pattern(std::vector<pattern_cell> cells) :
    _cells(std::move(cells))
{
}

} // namespace cas::automata
