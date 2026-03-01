#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "core/camera.h"

namespace cas::platform {
class Renderer {
public:
	explicit Renderer(SDL_Window* window);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Renderer(Renderer&& other) noexcept;
	Renderer& operator=(Renderer&& other) noexcept;

	void set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void present();

	void draw_rect(float x, float y, float w, float h);
	void fill_rect(float x, float y, float w, float h);
	void draw_hex(float cx, float cy, float size);
	void fill_hex(float cx, float cy, float size);

	SDL_Renderer* native() const { return m_handle; }
	void set_camera(Camera* camera) { m_camera = camera; }

private:
	SDL_Renderer* m_handle{};
	Camera* m_camera = nullptr;
	SDL_FColor m_draw_color{ 255.0f ,255.0f ,255.0f ,255.0f };
};
}

