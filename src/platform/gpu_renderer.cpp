#include <platform/gpu_renderer.h>

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <iomanip>
#include <iostream>


namespace cas::platform {
namespace {

[[nodiscard]] SDL_GPUTexture* create_texture(
    SDL_GPUDevice* device,
    Uint32 width,
    Uint32 height,
    SDL_GPUTextureFormat format,
    SDL_GPUTextureUsageFlags usage)
{
    SDL_GPUTextureCreateInfo info{};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = format;
    info.usage = usage;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &info);
    if (!texture) {
        throw std::runtime_error(
            std::string("No se pudo crear una textura SDL_GPU: ") + SDL_GetError());
    }
    return texture;
}

[[nodiscard]] std::vector<Uint8> load_shader_file(const char* filename)
{
    const char* base_path = SDL_GetBasePath();
    if (!base_path) {
        throw std::runtime_error(
            std::string("No se pudo obtener la ruta del ejecutable: ") + SDL_GetError());
    }

    char* path = SDL_strdup(
        (std::string(base_path) + "resources/shaders/" + filename).c_str());
    if (!path) {
        throw std::runtime_error("No se pudo construir la ruta del shader.");
    }

    std::size_t size = 0;
    void* file_data = SDL_LoadFile(path, &size);
    if (!file_data) {
        const std::string error =
            std::string("No se pudo cargar el shader ") + path + ": " + SDL_GetError();
        SDL_free(path);
        throw std::runtime_error(error);
    }

    const auto* bytes = static_cast<const Uint8*>(file_data);
    std::vector<Uint8> result(bytes, bytes + size);
    SDL_free(file_data);
    SDL_free(path);
    return result;
}

[[nodiscard]] SDL_GPUShader* create_shader(
    SDL_GPUDevice* device,
    const char* filename,
    const char* entrypoint,
    SDL_GPUShaderStage stage,
    Uint32 num_samplers,
    Uint32 num_uniform_buffers)
{
    const std::vector<Uint8> code = load_shader_file(filename);

    SDL_GPUShaderCreateInfo info{};
    info.code_size = code.size();
    info.code = code.data();
    info.entrypoint = entrypoint;
    info.format = SDL_GPU_SHADERFORMAT_DXIL;
    info.stage = stage;
    info.num_samplers = num_samplers;
    info.num_uniform_buffers = num_uniform_buffers;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &info);
    if (!shader) {
        throw std::runtime_error(
            std::string("No se pudo crear el shader ") + filename + ": " + SDL_GetError());
    }
    return shader;
}

} // namespace

gpu_renderer::gpu_renderer(gpu_device& device)
    : device_(device)
    , frame_(device)
    , output_texture_(device.native())
{
    try {
        create_pipeline();
        create_pixel_pipeline();
    }
    catch (...) {
        release_resources();
        throw;
    }
}

gpu_renderer::~gpu_renderer()
{
    frame_.cancel_frame();
    release_resources();
}

void gpu_renderer::begin_frame()
{
    frame_.begin_frame();
}

SDL_GPURenderPass* gpu_renderer::begin_render_pass(SDL_FColor clear_color)
{
    return frame_.begin_render_pass(clear_color);
}

void gpu_renderer::end_frame()
{
    frame_.end_frame();
}

void gpu_renderer::cancel_frame() noexcept
{
    frame_.cancel_frame();
}

void gpu_renderer::release_resources() noexcept
{
    SDL_GPUDevice* device = device_.native();

    if (transfer_buffer_) {
        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer_);
        transfer_buffer_ = nullptr;
    }
    if (input_texture_) {
        SDL_ReleaseGPUTexture(device, input_texture_);
        input_texture_ = nullptr;
    }

    output_texture_.reset(nullptr, {});

    if (sampler_) {
        SDL_ReleaseGPUSampler(device, sampler_);
        sampler_ = nullptr;
    }
    if (pipeline_) {
        SDL_ReleaseGPUGraphicsPipeline(device, pipeline_);
        pipeline_ = nullptr;
    }
    if (pixel_pipeline_) {
        SDL_ReleaseGPUGraphicsPipeline(device, pixel_pipeline_);
        pixel_pipeline_ = nullptr;
    }
    if (vertex_shader_) {
        SDL_ReleaseGPUShader(device, vertex_shader_);
        vertex_shader_ = nullptr;
    }
    if (fragment_shader_) {
        SDL_ReleaseGPUShader(device, fragment_shader_);
        fragment_shader_ = nullptr;
    }
    if (pixel_fragment_shader_) {
        SDL_ReleaseGPUShader(device, pixel_fragment_shader_);
        pixel_fragment_shader_ = nullptr;
    }

    viewport_width_ = viewport_height_ = 0;
    input_width_ = input_height_ = 0;
}

void gpu_renderer::resize(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);

    if (
        width == viewport_width_ &&
        height == viewport_height_ &&
        output_texture_.native()
        ) {
        return;
    }

    SDL_GPUTexture* replacement = create_texture(
        device_.native(),
        static_cast<Uint32>(width),
        static_cast<Uint32>(height),
        SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_COLOR_TARGET |
        SDL_GPU_TEXTUREUSAGE_SAMPLER
    );

    output_texture_.reset(
        replacement,
        math::vec2i{ width, height }
    );

    viewport_width_ = width;
    viewport_height_ = height;
}

void gpu_renderer::recreate_input_texture(int width, int height)
{
    if (width == input_width_ && height == input_height_
        && input_texture_ && transfer_buffer_) {
        return;
    }

    const std::size_t byte_count =
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    if (byte_count > std::numeric_limits<Uint32>::max()) {
        throw std::runtime_error("Los datos de entrada son demasiado grandes para el buffer GPU.");
    }

    SDL_GPUTexture* replacement_texture = create_texture(
        device_.native(), static_cast<Uint32>(width), static_cast<Uint32>(height),
        SDL_GPU_TEXTUREFORMAT_R8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);

    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = static_cast<Uint32>(byte_count);
    SDL_GPUTransferBuffer* replacement_buffer =
        SDL_CreateGPUTransferBuffer(device_.native(), &transfer_info);

    if (!replacement_buffer) {
        SDL_ReleaseGPUTexture(device_.native(), replacement_texture);
        throw std::runtime_error(
            std::string("No se pudo crear el buffer de transferencia: ") + SDL_GetError());
    }

    if (transfer_buffer_) {
        SDL_ReleaseGPUTransferBuffer(device_.native(), transfer_buffer_);
    }
    if (input_texture_) {
        SDL_ReleaseGPUTexture(device_.native(), input_texture_);
    }

    input_texture_ = replacement_texture;
    transfer_buffer_ = replacement_buffer;
    input_width_ = width;
    input_height_ = height;
}

void gpu_renderer::create_pipeline()
{
    SDL_GPUDevice* device = device_.native();
    vertex_shader_ = create_shader(
        device, "automaton_vertex.dxil", "vertex_main",
        SDL_GPU_SHADERSTAGE_VERTEX, 0, 0);
    fragment_shader_ = create_shader(
        device, "automaton_fragment.dxil", "fragment_main",
        SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);

    SDL_GPUSamplerCreateInfo sampler_info{};
    sampler_info.min_filter = SDL_GPU_FILTER_NEAREST;
    sampler_info.mag_filter = SDL_GPU_FILTER_NEAREST;
    sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.max_anisotropy = 1.0f;
    sampler_info.min_lod = 0.0f;
    sampler_info.max_lod = 0.0f;
    sampler_ = SDL_CreateGPUSampler(device, &sampler_info);
    if (!sampler_) {
        throw std::runtime_error(
            std::string("No se pudo crear el sampler: ") + SDL_GetError());
    }

    SDL_GPUColorTargetDescription color_target{};
    color_target.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    color_target.blend_state.color_write_mask =
        SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G
        | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
    color_target.blend_state.enable_color_write_mask = true;

    SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.vertex_shader = vertex_shader_;
    pipeline_info.fragment_shader = fragment_shader_;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipeline_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizer_state.enable_depth_clip = true;
    pipeline_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = &color_target;
    pipeline_ = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    if (!pipeline_) {
        throw std::runtime_error(
            std::string("No se pudo crear el pipeline GPU: ") + SDL_GetError());
    }
}

void gpu_renderer::create_pixel_pipeline()
{
    SDL_GPUDevice* device = device_.native();

    pixel_fragment_shader_ = create_shader(
        device,
        "automaton_pixels_fragment.dxil",
        "fragment_main",
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        1,
        1
    );

    SDL_GPUColorTargetDescription color_target{};
    color_target.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    color_target.blend_state.color_write_mask =
        SDL_GPU_COLORCOMPONENT_R |
        SDL_GPU_COLORCOMPONENT_G |
        SDL_GPU_COLORCOMPONENT_B |
        SDL_GPU_COLORCOMPONENT_A;
    color_target.blend_state.enable_color_write_mask = true;

    SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.vertex_shader = vertex_shader_;
    pipeline_info.fragment_shader = pixel_fragment_shader_;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipeline_info.rasterizer_state.front_face =
        SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizer_state.enable_depth_clip = true;
    pipeline_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = &color_target;

    pixel_pipeline_ = SDL_CreateGPUGraphicsPipeline(
        device,
        &pipeline_info
    );

    if (!pixel_pipeline_) {
        throw std::runtime_error(
            std::string("No se pudo crear el pipeline GPU de píxeles: ") +
            SDL_GetError()
        );
    }
}

void gpu_renderer::draw(
    std::span<const std::byte> data,
    int data_width,
    int data_height,
    std::span<const std::byte> uniforms,
    graphics::color clear_color)
{
    SDL_GPUCommandBuffer* command_buffer = frame_.command_buffer();
    if (command_buffer == nullptr) {
        throw std::invalid_argument("No hay un frame GPU activo.");
    }
    if (viewport_width_ <= 0 || viewport_height_ <= 0 || !output_texture_.native()) {
        return;
    }
    if (data_width <= 0 || data_height <= 0) {
        throw std::invalid_argument("Las dimensiones de los datos deben ser positivas.");
    }

    const std::size_t expected_size =
        static_cast<std::size_t>(data_width) * static_cast<std::size_t>(data_height);
    if (data.size() != expected_size) {
        throw std::invalid_argument("El tamaño de los datos no coincide con sus dimensiones.");
    }
    if (uniforms.empty()) {
        throw std::invalid_argument("El bloque de uniforms no puede estar vacío.");
    }

    recreate_input_texture(data_width, data_height);

    void* mapped = SDL_MapGPUTransferBuffer(device_.native(), transfer_buffer_, true);
    if (!mapped) {
        throw std::runtime_error(
            std::string("No se pudo mapear el buffer de transferencia: ") + SDL_GetError());
    }
    std::memcpy(mapped, data.data(), data.size());
    SDL_UnmapGPUTransferBuffer(device_.native(), transfer_buffer_);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = transfer_buffer_;
    source.offset = 0;

    SDL_GPUTextureRegion destination_region{};
    destination_region.texture = input_texture_;
    destination_region.mip_level = 0;
    destination_region.layer = 0;
    destination_region.x = destination_region.y = destination_region.z = 0;
    destination_region.w = static_cast<Uint32>(data_width);
    destination_region.h = static_cast<Uint32>(data_height);
    destination_region.d = 1;

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);
    if (!copy_pass) {
        throw std::runtime_error(
            std::string("No se pudo iniciar el copy pass: ") + SDL_GetError());
    }
    SDL_UploadToGPUTexture(copy_pass, &source, &destination_region, true);
    SDL_EndGPUCopyPass(copy_pass);

    SDL_GPUColorTargetInfo target{};
    target.texture = output_texture_.native();
    target.mip_level = 0;
    target.layer_or_depth_plane = 0;
    target.clear_color = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };
    target.load_op = SDL_GPU_LOADOP_CLEAR;
    target.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        command_buffer, &target, 1, nullptr);
    if (!render_pass) {
        throw std::runtime_error(
            std::string("No se pudo iniciar el render pass: ") + SDL_GetError());
    }

    SDL_BindGPUGraphicsPipeline(render_pass, pipeline_);
    const SDL_GPUViewport viewport{
        0.0f, 0.0f, static_cast<float>(viewport_width_),
        static_cast<float>(viewport_height_), 0.0f, 1.0f
    };
    SDL_SetGPUViewport(render_pass, &viewport);

    SDL_GPUTextureSamplerBinding binding{};
    binding.texture = input_texture_;
    binding.sampler = sampler_;
    SDL_BindGPUFragmentSamplers(render_pass, 0, &binding, 1);
    SDL_PushGPUFragmentUniformData(
        command_buffer, 0, uniforms.data(), uniforms.size());
    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
}

void gpu_renderer::draw_pixels(
    std::span<const std::byte> data,
    int data_width,
    int data_height,
    std::span<const std::byte> uniforms,
    graphics::color clear_color)
{
    SDL_GPUCommandBuffer* command_buffer = frame_.command_buffer();
    if (command_buffer == nullptr) {
        throw std::invalid_argument("No hay un frame GPU activo.");
    }
    if (viewport_width_ <= 0 || viewport_height_ <= 0 ||
        !output_texture_.native()) {
        return;
    }
    if (data_width <= 0 || data_height <= 0) {
        throw std::invalid_argument(
            "Las dimensiones de los datos deben ser positivas.");
    }

    const std::size_t expected_size =
        static_cast<std::size_t>(data_width) *
        static_cast<std::size_t>(data_height);

    if (data.size() != expected_size) {
        throw std::invalid_argument(
            "El tamaño de los datos no coincide con sus dimensiones.");
    }
    if (uniforms.empty()) {
        throw std::invalid_argument(
            "El bloque de uniforms no puede estar vacío.");
    }

    recreate_input_texture(data_width, data_height);

    void* mapped = SDL_MapGPUTransferBuffer(
        device_.native(), transfer_buffer_, true);

    if (!mapped) {
        throw std::runtime_error(
            std::string("No se pudo mapear el buffer de transferencia: ") +
            SDL_GetError());
    }

    std::memcpy(mapped, data.data(), data.size());
    SDL_UnmapGPUTransferBuffer(device_.native(), transfer_buffer_);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = transfer_buffer_;
    source.offset = 0;

    SDL_GPUTextureRegion destination_region{};
    destination_region.texture = input_texture_;
    destination_region.mip_level = 0;
    destination_region.layer = 0;
    destination_region.x = destination_region.y = destination_region.z = 0;
    destination_region.w = static_cast<Uint32>(data_width);
    destination_region.h = static_cast<Uint32>(data_height);
    destination_region.d = 1;

    SDL_GPUCopyPass* copy_pass =
        SDL_BeginGPUCopyPass(command_buffer);

    if (!copy_pass) {
        throw std::runtime_error(
            std::string("No se pudo iniciar el copy pass: ") +
            SDL_GetError());
    }

    SDL_UploadToGPUTexture(
        copy_pass, &source, &destination_region, true);
    SDL_EndGPUCopyPass(copy_pass);

    SDL_GPUColorTargetInfo target{};
    target.texture = output_texture_.native();
    target.mip_level = 0;
    target.layer_or_depth_plane = 0;
    target.clear_color = {
        clear_color.r, clear_color.g, clear_color.b, clear_color.a
    };
    target.load_op = SDL_GPU_LOADOP_CLEAR;
    target.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        command_buffer, &target, 1, nullptr);

    if (!render_pass) {
        throw std::runtime_error(
            std::string("No se pudo iniciar el render pass: ") +
            SDL_GetError());
    }

    if (!pixel_pipeline_) {
        throw std::runtime_error("pixel_pipeline_ es nulo.");
    }

    SDL_BindGPUGraphicsPipeline(render_pass, pixel_pipeline_);

    const SDL_GPUViewport viewport{
        0.0f,
        0.0f,
        static_cast<float>(viewport_width_),
        static_cast<float>(viewport_height_),
        0.0f,
        1.0f
    };

    SDL_SetGPUViewport(render_pass, &viewport);

    SDL_GPUTextureSamplerBinding binding{};
    binding.texture = input_texture_;
    binding.sampler = sampler_;

    SDL_BindGPUFragmentSamplers(render_pass, 0, &binding, 1);

    SDL_PushGPUFragmentUniformData(
        command_buffer, 0, uniforms.data(), uniforms.size());

    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
}

ImTextureID gpu_renderer::texture_id() const noexcept {
    return output_texture_.imgui_id();
}

} // namespace cas::platform