#include "rendering/automaton/hex_renderer.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <span>

namespace cas::rendering::automaton {

namespace {

constexpr float sqrt3 = 1.7320508075688772f;
constexpr float half_sqrt3 = 0.8660254037844386f;
constexpr float half = 0.5f;

} // namespace

hex_renderer::hex_renderer(
    cas::platform::renderer& renderer,
    cas::viewports::hex_layout layout
)
    : _renderer(renderer)
    , _texture(renderer)
    , _layout(layout)
{
}

void hex_renderer::resize(const math::vec2i& size) {
    _size = size;

    if (_size.x <= 0 || _size.y <= 0) {
        _pixels.clear();
        return;
    }

    _pixels.resize(
        static_cast<std::size_t>(_size.x) *
        static_cast<std::size_t>(_size.y)
    );

    static_cast<void>(_texture.resize(_size));
}

std::uint32_t hex_renderer::to_rgba32(const graphics::color& color) {
    const auto r = static_cast<std::uint32_t>(color.r * 255.0f);
    const auto g = static_cast<std::uint32_t>(color.g * 255.0f);
    const auto b = static_cast<std::uint32_t>(color.b * 255.0f);
    const auto a = static_cast<std::uint32_t>(color.a * 255.0f);

    return r | (g << 8) | (b << 16) | (a << 24);
}

void hex_renderer::clear(std::uint32_t pixel) {
    std::fill(_pixels.begin(), _pixels.end(), pixel);
}

void hex_renderer::draw_hex(
    const math::vec2f& center,
    float size,
    std::uint32_t pixel
) {
    const float min_y_f = center.y - size;
    const float max_y_f = center.y + size;

    const int min_y = std::max(
        0,
        static_cast<int>(std::floor(min_y_f))
    );

    const int max_y = std::min(
        _size.y - 1,
        static_cast<int>(std::ceil(max_y_f))
    );

    if (min_y > max_y)
        return;

    for (int y = min_y; y <= max_y; ++y) {
        const float dy = static_cast<float>(y) + half - center.y;
        const float abs_dy = std::abs(dy);

        if (abs_dy > size)
            continue;

        const float half_width = abs_dy <= size * half
            ? half_sqrt3 * size
            : sqrt3 * (size - abs_dy);

        const int left = std::max(
            0,
            static_cast<int>(std::ceil(center.x - half_width - half))
        );

        const int right = std::min(
            _size.x - 1,
            static_cast<int>(std::floor(center.x + half_width - half))
        );

        if (left > right)
            continue;

        const std::size_t row =
            static_cast<std::size_t>(y) *
            static_cast<std::size_t>(_size.x);

        std::fill(
            _pixels.begin() + row + static_cast<std::size_t>(left),
            _pixels.begin() + row + static_cast<std::size_t>(right) + 1,
            pixel
        );
    }
}

void hex_renderer::draw_line(
    const math::vec2f& start,
    const math::vec2f& end,
    std::uint32_t pixel
) {
    int x0 = static_cast<int>(std::lround(start.x));
    int y0 = static_cast<int>(std::lround(start.y));
    const int x1 = static_cast<int>(std::lround(end.x));
    const int y1 = static_cast<int>(std::lround(end.y));

    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;

    int error = dx + dy;

    while (true) {
        if (
            x0 >= 0 && x0 < _size.x &&
            y0 >= 0 && y0 < _size.y
        ) {
            const std::size_t index =
                static_cast<std::size_t>(y0) *
                static_cast<std::size_t>(_size.x) +
                static_cast<std::size_t>(x0);

            _pixels[index] = pixel;
        }

        if (x0 == x1 && y0 == y1)
            break;

        const int doubled_error = 2 * error;

        if (doubled_error >= dy) {
            error += dy;
            x0 += sx;
        }

        if (doubled_error <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

void hex_renderer::draw_square(
    const math::vec2f& center,
    float size,
    std::uint32_t pixel
) {
    if (size <= 0.0f)
        return;

    const float half_size = size * 0.5f;

    const int left = std::max(
        0,
        static_cast<int>(
            std::ceil(center.x - half_size - 0.5f)
            )
    );

    const int right = std::min(
        _size.x - 1,
        static_cast<int>(
            std::ceil(center.x + half_size - 0.5f)
            ) - 1
    );

    const int top = std::max(
        0,
        static_cast<int>(
            std::ceil(center.y - half_size - 0.5f)
            )
    );

    const int bottom = std::min(
        _size.y - 1,
        static_cast<int>(
            std::ceil(center.y + half_size - 0.5f)
            ) - 1
    );

    if (left > right || top > bottom)
        return;

    for (int y = top; y <= bottom; ++y) {
        const auto row_offset =
            static_cast<std::size_t>(y) * _size.x;

        std::fill(
            _pixels.begin() + row_offset + left,
            _pixels.begin() + row_offset + right + 1,
            pixel
        );
    }
}

void hex_renderer::draw_hex_outline(
    const math::vec2f& center,
    float size,
    std::uint32_t pixel
) {
    const float radius_x = half_sqrt3 * size;
    const float half_height = half * size;

    const math::vec2f vertices[] = {
        { center.x,             center.y - size },
        { center.x + radius_x,  center.y - half_height },
        { center.x + radius_x,  center.y + half_height },
        { center.x,             center.y + size },
        { center.x - radius_x,  center.y + half_height },
        { center.x - radius_x,  center.y - half_height }
    };

    for (std::size_t i = 0; i < 6; ++i) {
        draw_line(vertices[i], vertices[(i + 1) % 6], pixel);
    }
}

void hex_renderer::draw_cells(
    const automata::automaton& automaton,
    const config& config,
    const cas::viewports::camera& camera
) {
    const auto& current = automaton.storage().current();

    const int width = automaton.width();
    const int height = automaton.height();
    const int center_x = width / 2;
    const int center_y = height / 2;

    const float screen_size = config.cell_size * camera.zoom();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t index =
                static_cast<std::size_t>(y) *
                static_cast<std::size_t>(width) +
                static_cast<std::size_t>(x);

            const automata::cell_state state = current[index];

            if (state == 0)
                continue;

            const int q = x - center_x;
            const int r = y - center_y;

            const math::vec2f world = _layout.hex_to_world(
                q,
                r,
                config.cell_size,
                config.offset.x,
                config.offset.y
            );

            const math::vec2f screen = camera.world_to_screen(world);

            const float padding = screen_size;

            if (
                screen.x + padding < 0.0f ||
                screen.x - padding >= static_cast<float>(_size.x) ||
                screen.y + padding < 0.0f ||
                screen.y - padding >= static_cast<float>(_size.y)
            ) {
                continue;
            }

            const std::uint32_t pixel = to_rgba32(
                state == 1
                    ? config.alive_color
                    : config.persistent_color
            );

            draw_hex(screen, screen_size, pixel);
        }
    }
}

void hex_renderer::draw_grid(
    const automata::automaton& automaton,
    const config& config,
    const cas::viewports::camera& camera
) {
    const int width = automaton.width();
    const int height = automaton.height();
    const int center_x = width / 2;
    const int center_y = height / 2;

    const float screen_size = config.cell_size * camera.zoom();
    const float padding = screen_size;

    const std::uint32_t pixel = to_rgba32(config.grid_color);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int q = x - center_x;
            const int r = y - center_y;

            const math::vec2f world = _layout.hex_to_world(
                q,
                r,
                config.cell_size,
                config.offset.x,
                config.offset.y
            );

            const math::vec2f screen = camera.world_to_screen(world);

            if (
                screen.x + padding < 0.0f ||
                screen.x - padding >= static_cast<float>(_size.x) ||
                screen.y + padding < 0.0f ||
                screen.y - padding >= static_cast<float>(_size.y)
            ) {
                continue;
            }

            draw_hex_outline(screen, screen_size, pixel);
        }
    }
}

void hex_renderer::draw_linear_history(
    const viewports::linear_history& history,
    const config& config,
    const cas::viewports::camera& camera
) {
    constexpr int history_height =
        viewports::linear_history::max_generations;

    const int history_width = history.width();
    const int history_count = history.row_count();

    if (history_width <= 0 || history_count <= 0)
        return;

    const auto upload_size =
        static_cast<std::size_t>(history_width) *
        history_height;

    _history_cells.assign(upload_size, 0);

    const auto history_size =
        static_cast<std::size_t>(history_width) *
        history_count;

    history.copy_to(
        std::span<std::uint8_t>(
            _history_cells.data(),
            history_size
        )
    );

    const float cell_size = config.cell_size;
    const float screen_cell_size = cell_size * camera.zoom();

    if (screen_cell_size <= 0.0f)
        return;

    const auto alive_pixel = to_rgba32(config.alive_color);

    const float half_width = history_width * 0.5f;
    const float half_height = history_height * 0.5f;

    const float world_offset_y =
        config.offset.y + half_height * cell_size;

    for (int y = 0; y < history_height; ++y) {
        for (int x = 0; x < history_width; ++x) {
            const auto index =
                static_cast<std::size_t>(y) * history_width + x;

            if (_history_cells[index] == 0)
                continue;

            const math::vec2f world{
                (x - half_width) * cell_size + config.offset.x,
                (y - half_height) * cell_size + world_offset_y
            };

            const auto screen = camera.world_to_screen(world);

            // Evita rasterizar celdas que están completamente fuera
            // del área visible.
            const float half_screen_size =
                screen_cell_size * 0.5f;

            if (screen.x + half_screen_size < 0.0f ||
                screen.y + half_screen_size < 0.0f ||
                screen.x - half_screen_size >= _size.x ||
                screen.y - half_screen_size >= _size.y)
            {
                continue;
            }

            draw_square(
                screen,
                screen_cell_size,
                alive_pixel
            );
        }
    }
}

std::optional<std::reference_wrapper<platform::texture>>
hex_renderer::render(
    const automata::automaton& automaton,
    const viewports::linear_history& history,
    const config& config,
    const cas::viewports::camera& camera
) {
    if (
        _size.x <= 0 || _size.y <= 0 ||
        _pixels.empty() ||
        !_texture.resize(_size)
        ) {
        return std::nullopt;
    }

    clear(to_rgba32(config.background_color));

    if (automaton.topology().is_linear()) {
        if (history.empty())
            return std::nullopt;

        draw_linear_history(history, config, camera);
    }
    else {
        draw_cells(automaton, config, camera);
        draw_grid(automaton, config, camera);
    }

    const int pitch =
        _size.x * static_cast<int>(sizeof(std::uint32_t));

    if (!_texture.update(_pixels.data(), pitch))
        return std::nullopt;

    return std::ref(
        static_cast<platform::texture&>(_texture)
    );
}

} // namespace cas