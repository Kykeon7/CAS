
#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>

namespace cas::files::serialization {

using read_progress_callback = std::function<void(
    std::uint64_t completed,
    std::uint64_t total
    )>;

using progress_callback = std::function<void(
    std::uint64_t completed,
    std::uint64_t total,
    std::string_view stage
)>;

class progress_reporter {
public:
    progress_reporter() = default;

    explicit progress_reporter(
        progress_callback callback,
        std::uint64_t total = 1
    )
        : _callback(std::move(callback)),
          _total(std::max<std::uint64_t>(total, 1)),
          _length(_total) {
    }

    void report(
        std::uint64_t completed,
        std::uint64_t total,
        std::string_view stage = {}
    ) const {
        if (!_callback)
            return;

        const auto local_total =
            std::max<std::uint64_t>(total, 1);

        completed = std::min(completed, local_total);

        const auto scaled =
            static_cast<std::uint64_t>(
                (static_cast<long double>(completed)
                    / static_cast<long double>(local_total))
                * static_cast<long double>(_length)
            );

        report_absolute(scaled, stage);
    }

    void report_absolute(
        std::uint64_t completed,
        std::string_view stage = {}
    ) const {
        if (!_callback)
            return;

        const auto global_completed =
            _offset + std::min(completed, _length);

        _callback(global_completed, _total, stage);
    }

    [[nodiscard]] progress_reporter subrange(
        std::uint64_t offset,
        std::uint64_t length
    ) const {
        const auto bounded_offset =
            std::min(offset, _length);

        const auto bounded_length =
            std::min(length, _length - bounded_offset);

        return progress_reporter(
            _callback,
            _total,
            _offset + bounded_offset,
            bounded_length
        );
    }

    void complete(
        std::string_view stage = {}
    ) const {
        report_absolute(_length, stage);
    }

private:
    progress_reporter(
        progress_callback callback,
        std::uint64_t total,
        std::uint64_t offset,
        std::uint64_t length
    )
        : _callback(std::move(callback)),
          _total(total),
          _offset(offset),
          _length(length) {
    }

    progress_callback _callback;

    std::uint64_t _total = 1;
    std::uint64_t _offset = 0;
    std::uint64_t _length = 1;
};

} // namespace cas::files::serialization
