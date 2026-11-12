#pragma once

namespace cas::math {

/// @brief Calculates the mathematical modulo of two integers.
/// @param value Value to normalize.
/// @param modulus Positive modulus.
/// @return A value in the range [0, modulus).
/// @pre modulus must be greater than zero.
constexpr std::int64_t mod(
    std::int64_t value,
    std::int64_t modulus
) noexcept {
    const std::int64_t remainder = value % modulus;
    return remainder < 0 ? remainder + modulus : remainder;
}

} // namespace math

