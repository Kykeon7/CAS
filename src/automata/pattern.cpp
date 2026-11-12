#include <automata/pattern.h>

#include <utility>

namespace cas::automata {

pattern::pattern(std::vector<math::vec2i> offsets) :
    _offsets(std::move(offsets))
{
}

} // namespace cas::automata