#include "automaton/bit_array.h"

#include <stdexcept>

bit_array::bit_array(size_t bits)
    : size(bits), data((bits + 63) / 64, 0) {
}

bit_array::bit_array(size_t bits, bool init)
    : size(bits), data((bits + 63) / 64, init) {
}

bit_array::bit_array(const std::string& str)
    : size(str.size()), data((str.size() + 63) / 64, 0)
{
    from_string(str);
}

void bit_array::set(size_t pos, bool value) {
    if (pos >= size) throw std::out_of_range("BitArray::set");
    size_t idx = pos / 64;
    size_t off = pos % 64;
    if (value)
        data[idx] |= (1ULL << off);
    else
        data[idx] &= ~(1ULL << off);
}

bool bit_array::get(size_t pos) const {
    if (pos >= size) return false;
    size_t idx = pos / 64;
    size_t off = pos % 64;
    return (data[idx] >> off) & 1ULL;
}

void bit_array::toggle(size_t pos) {
    if (pos >= size) throw std::out_of_range("BitArray::toggle");
    size_t idx = pos / 64;
    size_t off = pos % 64;
    data[idx] ^= (1ULL << off);
}

// Operación AND con otro BitArray
bit_array& bit_array::operator&=(const bit_array& other) {
    if (size != other.size) throw std::invalid_argument("BitArray::&= size mismatch");
    for (size_t i = 0; i < data.size(); i++)
        data[i] &= other.data[i];

    return *this;
}

// Operación OR con otro BitArray
bit_array& bit_array::operator|=(const bit_array& other) {
    if (size != other.size) throw std::invalid_argument("BitArray::|= size mismatch");
    for (size_t i = 0; i < data.size(); i++)
        data[i] |= other.data[i];

    return *this;
}

std::string bit_array::to_string() const {
    std::string result;
    result.reserve(size);

    for (size_t i = 0; i < size; i++) 
        result.push_back(get(size - 1 - i) ? '1' : '0');
    
    return result;
}

void bit_array::from_string(const std::string& str) {
    if (str.size() != size) 
        throw std::invalid_argument("BitArray::fromString: size mismatch");
    
    for (size_t i = 0; i < size; i++) {
        char c = str[i];

        if (c == '1')
            set(size - 1 - i, true);
        else if (c == '0')
            set(size - 1 - i, false);
        else
            throw std::invalid_argument("BitArray::fromString: invalid character");
    }
}

void bit_array::clear() {
    std::fill(data.begin(), data.end(), 0ULL);
}

size_t bit_array::get_size() const noexcept { return size; }

