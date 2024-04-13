#include "Pipelines.hpp"

#define LUD_SLURPER_IMPLEMENTATION
#include "Util/lud_slurper.hpp"

namespace Ui::vkutil
{
vk::ShaderModule load_shader_module(const char* filepath, vk::Device device)
{
	auto code = Lud::Slurper::SlurpTo<uint32_t>(filepath, std::ios::binary);

	vk::ShaderModuleCreateInfo info({}, code);

	vk::ShaderModule module = device.createShaderModule(info);

	return module;
}
}