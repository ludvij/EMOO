#include "Shader.hpp"

vk::ShaderModule createShaderModule(const vk::Device device, const std::span<char>& code)
{
	vk::ShaderModuleCreateInfo create_info(
		vk::ShaderModuleCreateFlags(), 
		code.size(), 
		reinterpret_cast<uint32_t*>(code.data())
	);

	return device.createShaderModule(create_info);
}
