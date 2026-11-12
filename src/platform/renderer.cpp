#include <platform/renderer.h>

#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

#include <platform/color_conv.h>
#include <platform/vec_conv.h>

namespace cas::platform {

renderer::renderer(
    const window& window_,
    cas::viewports::camera& camera_
)
    : _camera(camera_)
{
    _handle = SDL_CreateRenderer(
        window_.native(),
        nullptr
    );

    if (!_handle)
        throw std::runtime_error(
            SDL_GetError()
        );
}

renderer::~renderer() {
    if (_handle)
        SDL_DestroyRenderer(_handle);
}

renderer::renderer(renderer&& other) noexcept
    : _handle(other._handle),
    _camera(other._camera),
    _draw_color(other._draw_color)
{
    other._handle = nullptr;
}

renderer& renderer::operator=(renderer&& other) noexcept {
    if (this != &other) {
        if (_handle)
            SDL_DestroyRenderer(_handle);

        _handle = other._handle;
        _camera = other._camera;
        _draw_color = other._draw_color;

        other._handle = nullptr;
    }

    return *this;
}

void renderer::set_draw_color(
    const graphics::color& color
) {
    _draw_color = color;

    const auto sdl = to_sdl(color);

    SDL_SetRenderDrawColor(
        _handle,
        sdl.r,
        sdl.g,
        sdl.b,
        sdl.a
    );
}

void renderer::set_draw_color(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t a
) {
    set_draw_color(
        graphics::color::from_rgba8(
            r,
            g,
            b,
            a
        )
    );
}

void renderer::clear(const graphics::color& color) {
    set_draw_color(color);

    SDL_RenderClear(_handle);
}

void renderer::clear(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t a
) {
    clear(
        graphics::color::from_rgba8(
            r,
            g,
            b,
            a
        )
    );
}

void renderer::present() {
    SDL_RenderPresent(_handle);
}

void renderer::draw_rect(
    const math::vec2f& pos,
    const math::vec2f& size
) {
    const math::vec2f screen =
        _camera.get().world_to_screen(pos);

    const float zoom =
        _camera.get().zoom();

    SDL_FRect rect{
        screen.x,
        screen.y,
        size.x * zoom,
        size.y * zoom
    };

    SDL_RenderRect(
        _handle,
        &rect
    );
}

void renderer::fill_rect(
    const math::vec2f& pos,
    const math::vec2f& size
) {
    const math::vec2f screen =
        _camera.get().world_to_screen(pos);

    const float zoom =
        _camera.get().zoom();

    SDL_FRect rect{
        screen.x,
        screen.y,
        size.x * zoom,
        size.y * zoom
    };

    SDL_RenderFillRect(
        _handle,
        &rect
    );
}

void renderer::draw_square(
    const math::vec2f& center,
    float size
) {
    const math::vec2f world{
        center.x - size * 0.5f,
        center.y - size * 0.5f
    };

    draw_rect(world, { size, size });
}

void renderer::fill_square(
    const math::vec2f& center,
    float size
) {
    const math::vec2f world{
        center.x - size * 0.5f,
        center.y - size * 0.5f
    };

    fill_rect(world, { size, size });
}

void renderer::draw_hex(
    const math::vec2f& center,
    float size
) {
    SDL_FPoint points[7]{};

    for (int i = 0; i < 6; ++i) {
        const float angle =
            static_cast<float>(
                std::numbers::pi
            ) / 180.f *
            (60.f * i - 30.f);

        const math::vec2f world{
            center.x + size * std::cos(angle),
            center.y + size * std::sin(angle)
        };

        const math::vec2f screen =
            _camera.get().world_to_screen(world);

        points[i] = {
            screen.x,
            screen.y
        };
    }

    points[6] = points[0];

    SDL_RenderLines(
        _handle,
        points,
        7
    );
}

void renderer::fill_hex(
    const math::vec2f& center,
    float size
) {
    SDL_FPoint outer[6]{};

    for (int i = 0; i < 6; ++i) {
        const float angle =
            static_cast<float>(
                std::numbers::pi
            ) / 180.f *
            (60.f * i - 30.f);

        const math::vec2f world{
            center.x + size * std::cos(angle),
            center.y + size * std::sin(angle)
        };

        const math::vec2f screen =
            _camera.get().world_to_screen(world);

        outer[i] = {
            screen.x,
            screen.y
        };
    }

    const math::vec2f screen_center =
        _camera.get().world_to_screen(center);

    const SDL_FColor sdl_color =
        to_sdl_fcolor(_draw_color);

    SDL_Vertex vertices[7]{};

    vertices[0].position = {
        screen_center.x,
        screen_center.y
    };

    vertices[0].color = sdl_color;
    vertices[0].tex_coord = {
        0.f,
        0.f
    };

    for (int i = 0; i < 6; ++i) {
        vertices[i + 1].position =
            outer[i];

        vertices[i + 1].color =
            sdl_color;

        vertices[i + 1].tex_coord = {
            0.f,
            0.f
        };
    }

    int indices[18]{};

    for (int i = 0; i < 6; ++i) {
        indices[i * 3 + 0] = 0;

        indices[i * 3 + 1] =
            i + 1;

        indices[i * 3 + 2] =
            (i == 5)
                ? 1
                : i + 2;
    }

    SDL_RenderGeometry(
        _handle,
        nullptr,
        vertices,
        7,
        indices,
        18
    );
}

void renderer::draw_geometry(
    std::span<const SDL_Vertex> vertices,
    std::span<const int> indices
) {
    if (vertices.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
        ) ||
        indices.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
            )) {
        throw std::length_error("Geometry exceeds SDL limits.");
    }

    SDL_RenderGeometry(
        _handle,
        nullptr,
        vertices.data(),
        static_cast<int>(vertices.size()),
        indices.data(),
        static_cast<int>(indices.size())
    );
}

void renderer::draw_texture(
    SDL_Texture* texture,
    const math::vec4& destination) 
{
    auto dest = to_sdl(destination);

    SDL_RenderTexture(
        _handle,
        texture,
        nullptr,
        &dest
    );
}

void renderer::draw_texture(SDL_Texture* texture) {
    SDL_SetTextureScaleMode(
        texture,
        SDL_SCALEMODE_NEAREST
    );

    SDL_RenderTexture(
        _handle,
        texture,
        nullptr,
        nullptr
    );
}

math::vec2f renderer::world_to_screen(
    const math::vec2f& world
) const {
    return _camera.get().world_to_screen(
        world
    );
}

float renderer::zoom() const {
    return _camera.get().zoom();
}

} // namespace cas::platform