#ifndef GRAPHICS_SHADER_HEADER
#define GRAPHICS_SHADER_HEADER

#include <vulkan/vulkan.hpp>

vk::ShaderModule createShaderModule(const vk::Device device, const std::span<char>& code);

#endif