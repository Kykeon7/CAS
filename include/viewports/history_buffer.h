
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace cas::viewports {

/**
 * Stores a fixed-capacity circular buffer of floating-point samples.
 *
 * New samples overwrite the oldest entries when the buffer reaches
 * capacity. The samples are exposed in chronological order through
 * a cached contiguous view.
 */
class history_buffer {
public:
    static constexpr std::size_t max_size = 500;

    /**
     * Appends a sample to the buffer.
     *
     * If the buffer is full, the oldest sample is overwritten.
     * Marks the chronological cache as dirty.
     *
     * @param value Sample to append.
     */
    void push(float value) noexcept {
        _data[_write_index] = value;

        _write_index = (_write_index + 1) % max_size;

        if (_size < max_size)
            ++_size;

        ++_total_samples;
        _dirty = true;
    }

    /**
     * Removes all samples from the buffer.
     *
     * Resets the write index, stored sample count, and total sample
     * count. Marks the chronological cache as dirty.
     */
    void clear() noexcept {
        _write_index = 0;
        _size = 0;
        _total_samples = 0;
        _dirty = true;
    }

    /**
     * Replaces the buffer contents with the supplied samples.
     *
     * If the input contains more than max_size samples, only the
     * most recent max_size samples are retained.
     *
     * @param values Samples to assign, in chronological order.
     */
    void assign(std::span<const float> values) noexcept {
        clear();

        for (const float value : values)
            push(value);
    }

    /**
     * Returns the number of currently stored samples.
     *
     * @return Number of stored samples.
     */
    [[nodiscard]]
    std::size_t size() const noexcept {
        return _size;
    }

    /**
     * Returns the maximum number of samples the buffer can store.
     *
     * @return Buffer capacity.
     */
    [[nodiscard]]
    constexpr std::size_t capacity() const noexcept {
        return max_size;
    }

    /**
     * Returns the total number of samples appended since the last clear.
     *
     * @return Total number of samples appended.
     */
    [[nodiscard]]
    std::uint64_t total_samples() const noexcept {
        return _total_samples;
    }

    /**
     * Checks whether the buffer contains any samples.
     *
     * @return true if the buffer is empty; otherwise, false.
     */
    [[nodiscard]]
    bool empty() const noexcept {
        return _size == 0;
    }

    /**
     * Returns the stored samples in chronological order.
     *
     * Rebuilds the internal cache if it has been invalidated by
     * a modification. The returned span remains valid until the
     * cache is modified by a subsequent call to this object.
     *
     * @return Read-only contiguous view of the stored samples.
     */
    [[nodiscard]]
    std::span<const float> samples() const noexcept {
        if (_dirty)
            rebuild_cache();

        return { _cache.data(), _size };
    }

private:
    /** Rebuilds the chronological cache from the circular buffer. */
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
