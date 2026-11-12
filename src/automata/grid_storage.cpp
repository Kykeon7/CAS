#include <automata/grid_storage.h>

#include <limits>
#include <stdexcept>
#include <utility>

namespace {

std::size_t calculate_size(int width, int height) {
    if (width < 1 || height < 1)
        throw std::invalid_argument(
            "Grid dimensions must be positive."
        );

    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);

    if (w > std::numeric_limits<std::size_t>::max() / h)
        throw std::length_error(
            "Grid dimensions exceed the maximum storage size."
        );

    return w * h;
}

} // namespace

namespace cas::automata {

grid_storage::grid_storage(int width, int height) :
    _width(width),
    _height(height),
    _current(calculate_size(width, height)),
    _next(_current.size())
{
}

void grid_storage::swap_buffers() {
    std::swap(_current, _next);
}

void grid_storage::resize(int width, int height) {
    if (width < 1 || height < 1)
        return;

    const std::size_t new_size =
        calculate_size(width, height);

    std::vector<cell_state> current(new_size);
    std::vector<cell_state> next(new_size);

    _current.swap(current);
    _next.swap(next);

    _width = width;
    _height = height;
}

} // namespace cas::automata