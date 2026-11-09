#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace cas::viewports {

class history_buffer {
public:
    static constexpr std::size_t max_size = 500;

    void push(float value) noexcept {
        _data[_write_index] = value;

        _write_index = (_write_index + 1) % max_size;

        if (_size < max_size)
            ++_size;

        ++_total_samples;
        _dirty = true;
    }

    void clear() noexcept {
        _write_index = 0;
        _size = 0;
        _total_samples = 0;
        _dirty = true;
    }

    void assign(std::span<const float> values) noexcept {
        clear();

        for (const float value : values)
            push(value);
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return _size;
    }

    [[nodiscard]]
    constexpr std::size_t capacity() const noexcept {
        return max_size;
    }

    [[nodiscard]]
    std::uint64_t total_samples() const noexcept {
        return _total_samples;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return _size == 0;
    }

    [[nodiscard]]
    std::span<const float> samples() const noexcept {
        if (_dirty)
            rebuild_cache();

        return { _cache.data(), _size };
    }

private:
    void rebuild_cache() const noexcept {
        const std::size_t oldest =
            (_write_index + max_size - _size) % max_size;

        for (std::size_t i = 0; i < _size; ++i) {
            _cache[i] = _data[(oldest + i) % max_size];
        }

        _dirty = false;
    }

private:
    std::vector<float> _data =
        std::vector<float>(max_size);

    std::size_t _write_index = 0;
    std::size_t _size = 0;
    std::uint64_t _total_samples = 0;

    mutable std::vector<float> _cache =
        std::vector<float>(max_size);

    mutable bool _dirty = true;
};

} // namespace cas::viewports