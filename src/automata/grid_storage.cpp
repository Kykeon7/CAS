#include <automata/grid_storage.h>

#include <algorithm>
#include <utility>

namespace cas::automata {

grid_storage::grid_storage(int width, int height)
    : _width(width)
    , _height(height)
    , _current(width* height)
    , _next(width* height)
{
}

cell_state grid_storage::get(int i) const {
    return _current[i];
}

cell_state grid_storage::get_next(int i) const {
    return _next[i];
}

void grid_storage::set_next(int i, cell_state state) {
    _next[i] = state;
}

void grid_storage::swap_buffers() {
    std::swap(_current, _next);
}

void grid_storage::set(size_t i, cell_state state) {
    _current[i] = state;
}

void grid_storage::resize(int width, int height) {
    if (width < 1 || height < 1)
        return;

    _width = width;
    _height = height;

    const auto size =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);

    _current.assign(size, cell_state{ 0 });
    _next.assign(size, cell_state{ 0 });
}

size_t grid_storage::size() const {
    return static_cast<size_t>(_width) * _height;
}

int grid_storage::width() const {
    return _width;
}

int grid_storage::height() const {
    return _height;
}

const std::vector<cell_state>& grid_storage::current() const {
    return _current;
}

std::vector<cell_state>& grid_storage::next() {
    return _next;
}

} // namespace cas::automata