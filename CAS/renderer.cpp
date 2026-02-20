#include "renderer.h"
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
    SDL_SetRenderDrawColor(m_handle, r, g, b, a);
}

void Renderer::draw_rect(float x, float y, float w, float h) {
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderRect(m_handle, &rect);
}
void Renderer::fill_rect(float x, float y, float w, float h) {
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderFillRect(m_handle, &rect);
}
void Renderer::draw_hex(float cx, float cy, float size) {
    SDL_FPoint points[7];

    for (int i = 0; i < 6; ++i) {
        float angle = static_cast<float>(std::numbers::pi) / 180.0f * (60.0f * i - 30.0f);

        points[i].x = cx + size * std::cos(angle);
        points[i].y = cy + size * std::sin(angle);
    }

    points[6] = points[0]; // cerrar polígono

    SDL_RenderLines(m_handle, points, 7);
    return;
    

}
void Renderer::fill_hex(float cx, float cy, float size)
{
    SDL_FPoint outer[6];

    for (int i = 0; i < 6; ++i) {
        float angle = static_cast<float>(std::numbers::pi) / 180.0f * (60.0f * i - 30.0f);

        outer[i].x = cx + size * std::cos(angle);
        outer[i].y = cy + size * std::sin(angle);
    }

    SDL_Vertex vertices[7];

    // Centro
    vertices[0].position = { cx, cy };
    vertices[0].color = { 255, 255, 255, 255 }; // usa el color actual del renderer
    vertices[0].tex_coord = { 0.0f, 0.0f };

    // 6 vértices externos
    for (int i = 0; i < 6; ++i) {
        vertices[i + 1].position = outer[i];
        vertices[i + 1].color = { 255, 255, 255, 255 };
        vertices[i + 1].tex_coord = { 0.0f, 0.0f };
    }

    int indices[18];

    for (int i = 0; i < 6; ++i) {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i == 5) ? 1 : i + 2;
    }

    SDL_RenderGeometry(
        m_handle,
        nullptr,
        vertices,
        7,
        indices,
        18
    );
}

}

