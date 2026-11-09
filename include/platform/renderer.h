#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <span>

#include "viewports/camera.h"
#include <graphics/color.h>
#include <graphics/colors.h>
#include <math/vec2.h>
#include <platform/window.h>
#include <math/vec4.h>

namespace cas::platform {

class renderer {

public:
    explicit renderer(
        const platform::window& window_,
        cas::viewports::camera& camera_
    );

    ~renderer();

    renderer(const renderer&) = delete;
    renderer& operator=(const renderer&) = delete;

    renderer(renderer&& other) noexcept;
    renderer& operator=(renderer&& other) noexcept;

    void set_draw_color(const graphics::color& color);
    void set_draw_color(
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a = 255
    );

    void clear(const graphics::color& color);
    void clear(
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a = 255
    );

    void present();

    void draw_rect(
        const math::vec2f& pos,
        const math::vec2f& size
    );

    void fill_rect(
        const math::vec2f& pos,
        const math::vec2f& size
    );

    void draw_square(
        const math::vec2f& center,
        float size
    );

    void fill_square(
        const math::vec2f& center,
        float size
    );

    void draw_hex(
        const math::vec2f& center,
        float size
    );

    void fill_hex(
        const math::vec2f& center,
        float size
    );

    void draw_geometry(
        std::span<const SDL_Vertex> vertices,
        std::span<const int> indices
    );

    void draw_texture(
        SDL_Texture* texture,
        const math::vec4& destination
    );

    void draw_texture(
        SDL_Texture* texture
    );

    SDL_Renderer* native() const {
        return _handle;
    }

    void set_camera(cas::viewports::camera& camera) {
        _camera = camera;
    }

    math::vec2f world_to_screen(
        const math::vec2f& world
    ) const;

    float zoom() const;

private:
    SDL_Renderer* _handle{};

    std::reference_wrapper<cas::viewports::camera> _camera;

    graphics::color _draw_color = graphics::white;
};

} // namespace cas::platform