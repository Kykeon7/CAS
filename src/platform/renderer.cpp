#include "platform/renderer.h"

#include <stdexcept>
#include <cmath>
#include <numbers>

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
	other.m_handle = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (this != &other) {
        if (m_handle)
            SDL_DestroyRenderer(m_handle);

        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }

    return *this;
}

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(m_handle, r, g, b, a);
    SDL_RenderClear(m_handle);
}

void Renderer::present() {
    SDL_RenderPresent(m_handle);
}

void Renderer::set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    m_draw_color = {
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    };

    SDL_SetRenderDrawColor(m_handle, r, g, b, a);
}


void Renderer::draw_rect(float x, float y, float w, float h) {
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderRect(m_handle, &rect);
}

void Renderer::fill_rect(float x, float y, float w, float h) {
    if (!m_camera)
        return;

    SDL_FPoint p = m_camera->world_to_screen(x, y);

    SDL_FRect rect{
        p.x,
        p.y,
        w * m_camera->zoom,
        h * m_camera->zoom
    };

    SDL_RenderFillRect(m_handle, &rect);
}

void Renderer::draw_hex(float cx, float cy, float size) {
    if (!m_camera)
        return;

    SDL_FPoint points[7];

    for (int i = 0; i < 6; ++i)
    {
        float angle = static_cast<float>(std::numbers::pi) / 180.0f * (60.0f * i - 30.0f);

        float wx = cx + size * std::cos(angle);
        float wy = cy + size * std::sin(angle);

        SDL_FPoint p = m_camera->world_to_screen(wx, wy);

        points[i] = p;
    }

    points[6] = points[0];

    SDL_RenderLines(m_handle, points, 7);
}

void Renderer::fill_hex(float cx, float cy, float size) {
    if (!m_camera)
        return;

    SDL_FPoint outer[6];

    for (int i = 0; i < 6; ++i)
    {
        float angle = static_cast<float>(std::numbers::pi) / 180.0f * (60.0f * i - 30.0f);

        float wx = cx + size * std::cos(angle);
        float wy = cy + size * std::sin(angle);

        outer[i] = m_camera->world_to_screen(wx, wy);
    }

    SDL_FPoint center = m_camera->world_to_screen(cx, cy);

    SDL_Vertex vertices[7];

    vertices[0].position = center;
    vertices[0].color = m_draw_color;
    vertices[0].tex_coord = { 0.0f, 0.0f };

    for (int i = 0; i < 6; ++i)
    {
        vertices[i + 1].position = outer[i];
        vertices[i + 1].color = m_draw_color;
        vertices[i + 1].tex_coord = { 0.0f, 0.0f };
    }

    int indices[18];

    for (int i = 0; i < 6; ++i)
    {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i == 5) ? 1 : i + 2;
    }

    SDL_RenderGeometry(m_handle, nullptr, vertices, 7, indices, 18);
}

}

