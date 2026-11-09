#include <rendering/resources.h>

#include <platform/renderer.h>
#include <platform/gpu_renderer.h>
#include <platform/gpu_device.h>

namespace cas::rendering {

	cpu_backend::cpu_backend() = default;
	cpu_backend::~cpu_backend() = default;

	cpu_backend::cpu_backend(cpu_backend&&) noexcept = default;

	cpu_backend& cpu_backend::operator=(cpu_backend&&) noexcept = default;

	gpu_backend::gpu_backend() = default;
	gpu_backend::~gpu_backend() = default;

	gpu_backend::gpu_backend(gpu_backend&&) noexcept = default;

	gpu_backend& gpu_backend::operator=(gpu_backend&&) noexcept = default;

} // namespace cas