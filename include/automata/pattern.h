#pragma once

#include <vector>

#include <math/vec2.h>

namespace cas::automata {

/// @brief Represents a collection of relative cell offsets.
/// @details Stores two-dimensional integer offsets that can be used
///          to describe a cell configuration or spatial arrangement.
class pattern {
public:
    pattern() = default;

    /// @brief Constructs a pattern from a collection of offsets.
    /// @param offsets Relative positions to store.
    explicit pattern(std::vector<math::vec2i> offsets);

    /// @brief Provides read-only access to the stored offsets.
    /// @return Constant reference to the offset collection.
    const std::vector<math::vec2i>& offsets() const noexcept {
        return _offsets;
    }

private:
    std::vector<math::vec2i> _offsets;
};

} // namespace cas::automata