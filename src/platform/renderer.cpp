#include "platform/renderer.h"

#include <stdexcept>
#include <cmath>
#include <numbers>
#include <core/graphics/color_conv.h>

namespace cas::platform {

Renderer::Renderer(SDL_Window* window) {
    m_handle = SDL_CreateRenderer(window, nullptr);

    if (!m_handle)
        throw std::runtime_error(SDL_GetError());
}

Renderer::~Renderer() {
    if (m_handle)
        SDL_DestroyRenderer(m_handle);
}

Renderer::Renderer(Renderer&& other) noexcept {
    m_handle = other.m_handle;
    m_camera = other.m_camera;

    other.m_handle = nullptr;
    other.m_camera = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (this != &other) {
        if (m_handle)
            SDL_DestroyRenderer(m_handle);

        m_handle = other.m_handle;
        m_camera = other.m_camera;

        other.m_handle = nullptr;
        other.m_camera = nullptr;
    }
    return *this;
}

void Renderer::set_draw_color(const Color& color) {
    m_draw_color = color;

    auto sdl = to_sdl(color);

    SDL_SetRenderDrawColor(
        m_handle,
        sdl.r,
        sdl.g,
        sdl.b,
        sdl.a
    );
}

void Renderer::set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    set_draw_color(Color::from_rgba8(r, g, b, a));
}

void Renderer::clear(const Color& color) {
    auto sdl = to_sdl(color);
    SDL_SetRenderDrawColor(m_handle, sdl.r, sdl.g, sdl.b, sdl.a);
    SDL_RenderClear(m_handle);
}

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    clear(Color::from_rgba8(r, g, b, a));
}

void Renderer::present() {
    SDL_RenderPresent(m_handle);
}

void Renderer::draw_rect(const Vec2& pos, const Vec2& size) {
    if (!m_camera)
        return;

    Vec2 screen = m_camera->world_to_screen(pos);
    float zoom = m_camera->zoom();

    SDL_FRect rect{
        screen.x,
        screen.y,
        size.x * zoom,
        size.y * zoom
    };

    SDL_RenderRect(m_handle, &rect);
}

void Renderer::fill_rect(const Vec2& pos, const Vec2& size) {
    if (!m_camera)
        return;

    Vec2 screen = m_camera->world_to_screen(pos);
    float zoom = m_camera->zoom();

    SDL_FRect rect{
        screen.x,
        screen.y,
        size.x * zoom,
        size.y * zoom
    };

    SDL_RenderFillRect(m_handle, &rect);
}

void Renderer::draw_hex(const Vec2& center, float size) {
    if (!m_camera)
        return;

    SDL_FPoint points[7]{};

    for (int i = 0; i < 6; ++i)
    {
        float angle = static_cast<float>(std::numbers::pi) / 180.f * (60.f * i - 30.f);

        Vec2 world{
            center.x + size * std::cos(angle),
            center.y + size * std::sin(angle)
        };

        Vec2 screen = m_camera->world_to_screen(world);

        points[i] = { screen.x, screen.y };
    }

    points[6] = points[0];

    SDL_RenderLines(m_handle, points, 7);
}

void Renderer::fill_hex(const Vec2& center, float size) {
    if (!m_camera)
        return;

    SDL_FPoint outer[6]{};

    for (int i = 0; i < 6; ++i)
    {
        float angle = static_cast<float>(std::numbers::pi) / 180.f * (60.f * i - 30.f);

        Vec2 world{
            center.x + size * std::cos(angle),
            center.y + size * std::sin(angle)
        };

        Vec2 screen = m_camera->world_to_screen(world);
        outer[i] = { screen.x, screen.y };
    }

    Vec2 screen_center = m_camera->world_to_screen(center);

    SDL_FColor sdl_color = to_sdl_fcolor(m_draw_color);

    SDL_Vertex vertices[7]{};

    vertices[0].position = { screen_center.x, screen_center.y };
    vertices[0].color = sdl_color;
    vertices[0].tex_coord = { 0.f, 0.f };

    for (int i = 0; i < 6; ++i)
    {
        vertices[i + 1].position = outer[i];
        vertices[i + 1].color = sdl_color;
        vertices[i + 1].tex_coord = { 0.f, 0.f };
    }

    int indices[18]{};

    for (int i = 0; i < 6; ++i)
    {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i == 5) ? 1 : i + 2;
    }

    SDL_RenderGeometry(m_handle, nullptr, vertices, 7, indices, 18);
}

}