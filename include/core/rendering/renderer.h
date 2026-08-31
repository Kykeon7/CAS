#pragma once
#include <SDL3/SDL.h>
#include "core/camera.h"
#include <core/graphics/color.h>
#include <core/math/vec2.h>

namespace cas::platform {

class Renderer {
public:
    explicit Renderer(SDL_Window* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    void set_draw_color(const color& color);
    void set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void clear(const color& color);
    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void present();

    void draw_rect(const Vec2& pos, const Vec2& size);
    void fill_rect(const Vec2& pos, const Vec2& size);
    void draw_hex(const Vec2& center, float size);
    void fill_hex(const Vec2& center, float size);

    SDL_Renderer* native() const { return m_handle; }
    void set_camera(Camera* camera) { m_camera = camera; }

private:
    SDL_Renderer* m_handle{};
    Camera* m_camera = nullptr;

    color m_draw_color{ 1.f, 1.f, 1.f, 1.f }; // blanco correcto
};

}