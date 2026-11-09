#include <rendering/automaton/gpu_renderer.h>

#include <platform/gpu_renderer.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace cas::rendering::automaton {

namespace {

struct alignas(16) gpu_render_config {
    float viewport_size[2];
    float camera_position[2];
    float zoom;
    float cell_size;
    float world_offset[2];
    std::int32_t grid_width;
    std::int32_t grid_height;
    float padding[2];
    float background_color[4];
    float grid_color[4];
    float alive_color[4];
    float persistent_color[4];
};

static_assert(sizeof(gpu_render_config) == 112);

struct alignas(16) pixel_config {
    std::int32_t grid_width = 0;
    std::int32_t grid_height = 0;
    float padding[2]{};

    float viewport_size[2]{};
    float camera_position[2]{};

    float zoom = 1.0f;
    float cell_size = 1.0f;
    float world_offset[2]{};

    float background_color[4]{};
    float alive_color[4]{};
};

static_assert(sizeof(pixel_config) == 80);

void copy_color(float* destination, const graphics::color& source) {
    destination[0] = source.r;
    destination[1] = source.g;
    destination[2] = source.b;
    destination[3] = source.a;
}

} // namespace

gpu_renderer::gpu_renderer(
    platform::gpu_renderer& renderer
) noexcept
    : _renderer(renderer)
{
}

void gpu_renderer::resize(const math::vec2i& size) {
    if (size.x <= 0 || size.y <= 0) {
        _viewport_size = {};
        return;
    }

    _renderer.resize(size.x, size.y);
    _viewport_size = size;
}


std::optional<std::reference_wrapper<platform::texture>>
gpu_renderer::render(
    const automata::automaton& automaton,
    const viewports::linear_history& history,
    const config& config,
    const cas::viewports::camera& camera
) {
    if (_viewport_size.x <= 0 || _viewport_size.y <= 0)
        return std::nullopt;

    auto& output_texture = _renderer.output_texture();

    if (!output_texture.native())
        return std::nullopt;

    const int grid_width = automaton.width();
    const int grid_height = automaton.height();

    if (grid_width <= 0 || grid_height <= 0) {
        throw std::runtime_error(
            "Las dimensiones del autómata deben ser positivas."
        );
    }

    const auto& states = automaton.storage().current();

    const std::size_t expected_size =
        static_cast<std::size_t>(grid_width) *
        static_cast<std::size_t>(grid_height);

    if (states.size() != expected_size) {
        throw std::runtime_error(
            "El almacenamiento del autómata no coincide con sus dimensiones."
        );
    }

    std::vector<std::byte> data;
    data.reserve(states.size());

    for (const auto state : states) {
        data.push_back(
            static_cast<std::byte>(static_cast<std::uint8_t>(state))
        );
    }

    const auto data_bytes = std::span<const std::byte>(data);

    const bool is_linear = automaton.topology().is_linear();

    if (is_linear) {
        constexpr int history_height =
            viewports::linear_history::max_generations;

        const int history_width = history.width();
        const int history_count = history.row_count();

        if (history_width <= 0 || history_count <= 0)
            return std::nullopt;

        const std::size_t upload_size =
            static_cast<std::size_t>(history_width) * history_height;

        _linear_history_upload.assign(upload_size, 0);

        const std::size_t history_size =
            static_cast<std::size_t>(history_width) * history_count;

        history.copy_to(
            std::span<std::uint8_t>(
                _linear_history_upload.data(),
                history_size
            )
        );

        pixel_config uniforms{};

        uniforms.grid_width = history_width;
        uniforms.grid_height = history_height;

        uniforms.viewport_size[0] =
            static_cast<float>(_viewport_size.x);
        uniforms.viewport_size[1] =
            static_cast<float>(_viewport_size.y);

        uniforms.camera_position[0] = camera.position().x;
        uniforms.camera_position[1] = camera.position().y;
        uniforms.zoom = camera.zoom();

        uniforms.cell_size = config.cell_size;
        uniforms.world_offset[0] = config.offset.x;
        uniforms.world_offset[1] =
            config.offset.y + history_height * 0.5f * config.cell_size;

        copy_color(uniforms.background_color, config.background_color);
        copy_color(uniforms.alive_color, config.alive_color);

        const auto uniform_bytes = std::as_bytes(
            std::span{ &uniforms, std::size_t{1} }
        );

        const auto data = std::as_bytes(
            std::span<const std::uint8_t>{_linear_history_upload}
        );

        _renderer.draw_pixels(
            data,
            history_width,
            history_height,
            uniform_bytes,
            config.background_color
        );
    }
    else {
        gpu_render_config uniforms{};

        uniforms.viewport_size[0] =
            static_cast<float>(_viewport_size.x);
        uniforms.viewport_size[1] =
            static_cast<float>(_viewport_size.y);

        uniforms.camera_position[0] = camera.position().x;
        uniforms.camera_position[1] = camera.position().y;
        uniforms.zoom = camera.zoom();

        uniforms.cell_size = config.cell_size;
        uniforms.world_offset[0] = config.offset.x;
        uniforms.world_offset[1] = config.offset.y;

        uniforms.grid_width = grid_width;
        uniforms.grid_height = grid_height;

        copy_color(uniforms.background_color, config.background_color);
        copy_color(uniforms.grid_color, config.grid_color);
        copy_color(uniforms.alive_color, config.alive_color);
        copy_color(uniforms.persistent_color, config.persistent_color);

        const auto uniform_bytes = std::as_bytes(
            std::span{ &uniforms, std::size_t{1} }
        );

        _renderer.draw(
            data_bytes,
            grid_width,
            grid_height,
            uniform_bytes,
            config.background_color
        );
    }

    return std::ref(
        static_cast<platform::texture&>(output_texture)
    );

}

} // namespace cas