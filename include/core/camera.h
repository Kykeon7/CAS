#pragma once

#include <SDL3/SDL_rect.h>

struct Camera {
    float x = 0.0f;
    float y = 0.0f;
    float zoom = 1.0f;

    SDL_FPoint world_to_screen(float wx, float wy) const {
        return {
            (wx - x) * zoom,
            (wy - y) * zoom
        };
    }
};

