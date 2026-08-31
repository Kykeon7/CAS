#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

class bit_array {
public:
    explicit bit_array(size_t bits);
    explicit bit_array(size_t bits, bool init);
    explicit bit_array(const std::string& str);

    void set(size_t pos, bool value = true);
    bool get(size_t pos) const;
    void toggle(size_t pos);
    bit_array& operator&=(const bit_array& other);
    bit_array& operator|=(const bit_array& other);
    std::string to_string() const;
    void from_string(const std::string& str);
    void clear();

    size_t get_size() const noexcept;

    const std::vector<uint64_t>& blocks() const { return data; }
    std::vector<uint64_t>& blocks() { return data; }

private:
    size_t size;
    std::vector<uint64_t> data;
};

inline std::ostream& operator<<(std::ostream& os, const bit_array& arr) noexcept {
    return os << arr.to_string();
}
