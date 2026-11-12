#include <viewports/linear_history.h>

#include <algorithm>
#include <cstddef>

namespace cas::viewports {

void linear_history::record(
    const automata::automaton& automaton
) {
    if (!automaton.topology().is_linear()) {
        clear();
        _revision = automaton.configuration_revision();
        return;
    }

    synchronize(automaton);

    const int generation = automaton.generation_count();

    if (_last_recorded_generation == generation)
        return;

    const auto& current = automaton.storage().current();

    if (static_cast<int>(current.size()) != _width)
        return;

    const std::size_t row_offset =
        static_cast<std::size_t>(_head) *
        static_cast<std::size_t>(_width);

    for (int x = 0; x < _width; ++x) {
        _data[row_offset + static_cast<std::size_t>(x)] =
            static_cast<std::uint8_t>(current[x]);
    }

    _head = (_head + 1) % max_generations;
    _count = std::min(_count + 1, max_generations);

    _last_recorded_generation = generation;
}

void linear_history::clear() noexcept {
    _data.clear();

    _width = 0;
    _head = 0;
    _count = 0;

    _revision = std::numeric_limits<std::uint64_t>::max();
    _last_recorded_generation = -1;
}

int linear_history::width() const noexcept {
    return _width;
}

int linear_history::row_count() const noexcept {
    return _count;
}

bool linear_history::empty() const noexcept {
    return _count == 0;
}

void linear_history::copy_to(
    std::span<std::uint8_t> destination
) const {
    const std::size_t required_size =
        static_cast<std::size_t>(_width) *
        static_cast<std::size_t>(_count);

    if (destination.size() < required_size)
        return;

    if (_count == 0)
        return;

    const int oldest_row =
        _count == max_generations ? _head : 0;

    for (int y = 0; y < _count; ++y) {
        const int source_row =
            (oldest_row + y) % max_generations;

        const std::size_t source_offset =
            static_cast<std::size_t>(source_row) * _width;

        const std::size_t destination_offset =
            static_cast<std::size_t>(y) * _width;

        std::copy_n(
            _data.begin() + source_offset,
            _width,
            destination.begin() + destination_offset
        );
    }
}

std::vector<std::uint8_t>
linear_history::chronological_data() const {
    std::vector<std::uint8_t> result(
        static_cast<std::size_t>(_width) * _count
    );

    copy_to(result);

    return result;
}

bool linear_history::restore(
    const automata::automaton& automaton,
    std::span<const std::uint8_t> data
) {
    if (!automaton.topology().is_linear())
        return false;

    const int width = automaton.width();

    if (width <= 0)
        return false;

    if (data.size() % static_cast<std::size_t>(width) != 0)
        return false;

    const std::size_t rows =
        data.size() / static_cast<std::size_t>(width);

    if (rows > max_generations)
        return false;

    if (automaton.storage().current().size() !=
        static_cast<std::size_t>(width)) {
        return false;
    }

    std::vector<std::uint8_t> restored_data(
        static_cast<std::size_t>(width) * max_generations,
        0
    );

    std::copy(
        data.begin(),
        data.end(),
        restored_data.begin()
    );

    _data = std::move(restored_data);

    _width = width;
    _count = static_cast<int>(rows);
    _head = _count % max_generations;

    _revision = automaton.configuration_revision();

    // La última generación ya está representada en el historial.
    _last_recorded_generation = automaton.generation_count();

    return true;
}

void linear_history::synchronize(
    const automata::automaton& automaton
) {
    const int width = automaton.width();
    const auto revision = automaton.configuration_revision();

    if (_width == width && _revision == revision)
        return;

    _width = width;

    _data.assign(
        static_cast<std::size_t>(_width) * max_generations,
        0
    );

    _head = 0;
    _count = 0;

    _revision = revision;
    _last_recorded_generation = -1;
}

} // namespace cas::viewports
