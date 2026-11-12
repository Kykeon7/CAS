#include <platform/gpu_renderer.h>

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>   

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

std::vector<Uint8> load_shader_file(const char* filename) {
    const char* base_path = SDL_GetBasePath();

    if (!base_path)
        throw std::runtime_error("No se pudo obtener la ruta base del ejecutable.");

    const std::string path =
        std::string(base_path) + "resources/shaders/" + filename;

    size_t size = 0;
    std::unique_ptr<void, decltype(&SDL_free)> file_data(
        SDL_LoadFile(path.c_str(), &size),
        SDL_free
    );

    if (!file_data)
        throw std::runtime_error(
            "No se pudo cargar el shader: " + path + ". " + SDL_GetError()
        );

    if (size == 0)
        throw std::runtime_error("El archivo de shader está vacío: " + path);

    const auto* bytes = static_cast<const Uint8*>(file_data.get());

    return { bytes, bytes + size };
}

SDL_GPUShader* create_shader(
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

    if (!shader)
        throw std::runtime_error(
            "No se pudo crear el shader: " + std::string(filename) +
            ". " + SDL_GetError()
        );

    return shader;
}

} // namespace

gpu_renderer::gpu_renderer(gpu_device& device)
    : _device(device)
    , _frame(device)
    , _output_texture(device.native())
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
    _frame.cancel_frame();
    release_resources();
}

void gpu_renderer::begin_frame()
{
    _frame.begin_frame();
}

SDL_GPURenderPass* gpu_renderer::begin_render_pass(SDL_FColor clear_color)
{
    return _frame.begin_render_pass(clear_color);
}

void gpu_renderer::end_frame()
{
    _frame.end_frame();
}

void gpu_renderer::cancel_frame() noexcept
{
    _frame.cancel_frame();
}

void gpu_renderer::release_resources() noexcept
{
    SDL_GPUDevice* device = _device.native();

    if (_transfer_buffer) {
        SDL_ReleaseGPUTransferBuffer(device, _transfer_buffer);
        _transfer_buffer = nullptr;
    }
    if (_input_texture) {
        SDL_ReleaseGPUTexture(device, _input_texture);
        _input_texture = nullptr;
    }

    _output_texture.reset(nullptr, {});

    if (_sampler) {
        SDL_ReleaseGPUSampler(device, _sampler);
        _sampler = nullptr;
    }
    if (_pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(device, _pipeline);
        _pipeline = nullptr;
    }
    if (_pixel_pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(device, _pixel_pipeline);
        _pixel_pipeline = nullptr;
    }
    if (_vertex_shader) {
        SDL_ReleaseGPUShader(device, _vertex_shader);
        _vertex_shader = nullptr;
    }
    if (_fragment_shader) {
        SDL_ReleaseGPUShader(device, _fragment_shader);
        _fragment_shader = nullptr;
    }
    if (_pixel_fragment_shader) {
        SDL_ReleaseGPUShader(device, _pixel_fragment_shader);
        _pixel_fragment_shader = nullptr;
    }

    _viewport_width = _viewport_height = 0;
    _input_width = _input_height = 0;
}

void gpu_renderer::resize(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);

    if (
        width == _viewport_width &&
        height == _viewport_height &&
        _output_texture.native()
        ) {
        return;
    }

    SDL_GPUTexture* replacement = create_texture(
        _device.native(),
        static_cast<Uint32>(width),
        static_cast<Uint32>(height),
        SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_COLOR_TARGET |
        SDL_GPU_TEXTUREUSAGE_SAMPLER
    );

    _output_texture.reset(
        replacement,
        math::vec2i{ width, height }
    );

    _viewport_width = width;
    _viewport_height = height;
}

void gpu_renderer::recreate_input_texture(int width, int height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument(
            "Las dimensiones de la textura de entrada deben ser positivas."
        );
    }
    
    const bool input_resources_match =
        width == _input_width &&
        height == _input_height &&
        _input_texture != nullptr &&
        _transfer_buffer != nullptr;

    if (input_resources_match)
        return;

    if (static_cast<std::size_t>(width) >
        std::numeric_limits<Uint32>::max() /
        static_cast<std::size_t>(height)) {
        throw std::length_error(
            "Los datos de entrada exceden el tamaño máximo del buffer GPU."
        );
    }

    const std::size_t byte_count =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);

    SDL_GPUTexture* replacement_texture = create_texture(
        _device.native(),
        static_cast<Uint32>(width),
        static_cast<Uint32>(height),
        SDL_GPU_TEXTUREFORMAT_R8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER
    );

    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = static_cast<Uint32>(byte_count);

    SDL_GPUTransferBuffer* replacement_buffer =
        SDL_CreateGPUTransferBuffer(_device.native(), &transfer_info);

    if (!replacement_buffer) {
        SDL_ReleaseGPUTexture(_device.native(), replacement_texture);

        throw std::runtime_error(
            std::string("No se pudo crear el buffer de transferencia: ") +
            SDL_GetError()
        );
    }

    if (_transfer_buffer)
        SDL_ReleaseGPUTransferBuffer(_device.native(), _transfer_buffer);

    if (_input_texture)
        SDL_ReleaseGPUTexture(_device.native(), _input_texture);

    _input_texture = replacement_texture;
    _transfer_buffer = replacement_buffer;
    _input_width = width;
    _input_height = height;
}

void gpu_renderer::create_pipeline() {
    SDL_GPUDevice* device = _device.native();

    _vertex_shader = create_shader(
        device,
        "automaton_vertex.dxil",
        "vertex_main",
        SDL_GPU_SHADERSTAGE_VERTEX,
        0,
        0
    );

    _fragment_shader = create_shader(
        device,
        "automaton_fragment.dxil",
        "fragment_main",
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        1,
        1
    );

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

    _sampler = SDL_CreateGPUSampler(device, &sampler_info);

    if (!_sampler) 
        throw std::runtime_error(
            std::string("No se pudo crear el sampler: ") + SDL_GetError()
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
    pipeline_info.vertex_shader = _vertex_shader;
    pipeline_info.fragment_shader = _fragment_shader;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipeline_info.rasterizer_state.front_face =
        SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizer_state.enable_depth_clip = true;
    pipeline_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = &color_target;

    _pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);

    if (!_pipeline) 
        throw std::runtime_error(
            std::string("No se pudo crear el pipeline GPU: ") + SDL_GetError()
        );
    
}

void gpu_renderer::create_pixel_pipeline() {
    SDL_GPUDevice* device = _device.native();

    _pixel_fragment_shader = create_shader(
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
    pipeline_info.vertex_shader = _vertex_shader;
    pipeline_info.fragment_shader = _pixel_fragment_shader;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipeline_info.rasterizer_state.front_face =
        SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizer_state.enable_depth_clip = true;
    pipeline_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = &color_target;

    _pixel_pipeline = SDL_CreateGPUGraphicsPipeline(
        device,
        &pipeline_info
    );

    if (!_pixel_pipeline) 
        throw std::runtime_error(
            std::string("No se pudo crear el pipeline GPU de píxeles: ") +
            SDL_GetError()
        );
    
}

void gpu_renderer::draw(
    std::span<const std::byte> data,
    int data_width,
    int data_height,
    std::span<const std::byte> uniforms,
    graphics::color clear_color)
{
    SDL_GPUCommandBuffer* command_buffer = _frame.command_buffer();
    if (command_buffer == nullptr) {
        throw std::invalid_argument("No hay un frame GPU activo.");
    }
    if (_viewport_width <= 0 || _viewport_height <= 0 || !_output_texture.native()) {
        return;
    }
    if (data_width <= 0 || data_height <= 0) {
        throw std::invalid_argument("Las dimensiones de los datos deben ser positivas.");
    }

    if (static_cast<std::size_t>(data_width) >
        std::numeric_limits<Uint32>::max() /
        static_cast<std::size_t>(data_height)) {
        throw std::length_error(
            "Los datos de entrada exceden el tamaño máximo del buffer GPU."
        );
    }

    const std::size_t expected_size =
        static_cast<std::size_t>(data_width) *
        static_cast<std::size_t>(data_height);

    if (data.size() != expected_size) {
        throw std::invalid_argument("El tamaño de los datos no coincide con sus dimensiones.");
    }
    if (uniforms.empty() ||
        uniforms.size() > std::numeric_limits<Uint32>::max()) {
        throw std::invalid_argument(
            "El tamaño del bloque de uniforms no es válido."
        );
    }

    recreate_input_texture(data_width, data_height);

    void* mapped = SDL_MapGPUTransferBuffer(_device.native(), _transfer_buffer, true);
    if (!mapped) {
        throw std::runtime_error(
            std::string("No se pudo mapear el buffer de transferencia: ") + SDL_GetError());
    }
    std::memcpy(mapped, data.data(), data.size());
    SDL_UnmapGPUTransferBuffer(_device.native(), _transfer_buffer);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = _transfer_buffer;
    source.offset = 0;

    SDL_GPUTextureRegion destination_region{};
    destination_region.texture = _input_texture;
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
    target.texture = _output_texture.native();
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

    SDL_BindGPUGraphicsPipeline(render_pass, _pipeline);
    const SDL_GPUViewport viewport{
        0.0f, 0.0f, static_cast<float>(_viewport_width),
        static_cast<float>(_viewport_height), 0.0f, 1.0f
    };
    SDL_SetGPUViewport(render_pass, &viewport);

    SDL_GPUTextureSamplerBinding binding{};
    binding.texture = _input_texture;
    binding.sampler = _sampler;
    SDL_BindGPUFragmentSamplers(render_pass, 0, &binding, 1);
    SDL_PushGPUFragmentUniformData(
        command_buffer,
        0,
        uniforms.data(),
        static_cast<Uint32>(uniforms.size())
    );
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
    SDL_GPUCommandBuffer* command_buffer = _frame.command_buffer();

    if (command_buffer == nullptr) {
        throw std::invalid_argument("No hay un frame GPU activo.");
    }
    if (!_pixel_pipeline) {
        throw std::runtime_error("_pixel_pipeline es nulo.");
    }
    if (_viewport_width <= 0 || _viewport_height <= 0 ||
        !_output_texture.native()) {
        return;
    }
    if (data_width <= 0 || data_height <= 0) {
        throw std::invalid_argument(
            "Las dimensiones de los datos deben ser positivas.");
    }

    if (static_cast<std::size_t>(data_width) >
        std::numeric_limits<Uint32>::max() /
        static_cast<std::size_t>(data_height)) {
        throw std::length_error(
            "Los datos de entrada exceden el tamaño máximo del buffer GPU."
        );
    }

    const std::size_t expected_size =
        static_cast<std::size_t>(data_width) *
        static_cast<std::size_t>(data_height);

    if (data.size() != expected_size) {
        throw std::invalid_argument(
            "El tamaño de los datos no coincide con sus dimensiones.");
    }
    if (uniforms.empty() ||
        uniforms.size() > std::numeric_limits<Uint32>::max()) {
        throw std::invalid_argument(
            "El tamaño del bloque de uniforms no es válido."
        );
    }

    recreate_input_texture(data_width, data_height);

    void* mapped = SDL_MapGPUTransferBuffer(
        _device.native(), _transfer_buffer, true);

    if (!mapped) {
        throw std::runtime_error(
            std::string("No se pudo mapear el buffer de transferencia: ") +
            SDL_GetError());
    }

    std::memcpy(mapped, data.data(), data.size());
    SDL_UnmapGPUTransferBuffer(_device.native(), _transfer_buffer);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = _transfer_buffer;
    source.offset = 0;

    SDL_GPUTextureRegion destination_region{};
    destination_region.texture = _input_texture;
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
    target.texture = _output_texture.native();
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

    SDL_BindGPUGraphicsPipeline(render_pass, _pixel_pipeline);

    const SDL_GPUViewport viewport{
        0.0f,
        0.0f,
        static_cast<float>(_viewport_width),
        static_cast<float>(_viewport_height),
        0.0f,
        1.0f
    };

    SDL_SetGPUViewport(render_pass, &viewport);

    SDL_GPUTextureSamplerBinding binding{};
    binding.texture = _input_texture;
    binding.sampler = _sampler;

    SDL_BindGPUFragmentSamplers(render_pass, 0, &binding, 1);

    SDL_PushGPUFragmentUniformData(
        command_buffer,
        0,
        uniforms.data(),
        static_cast<Uint32>(uniforms.size())
    );

    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
}

ImTextureID gpu_renderer::texture_id() const noexcept {
    return _output_texture.imgui_id();
}

} // namespace cas::platform