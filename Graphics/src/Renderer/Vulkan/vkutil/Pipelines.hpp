#ifndef GRAPHICS_RENDERER_PIPELINES_HEADER
#define GRAPHICS_RENDERER_PIPELINES_HEADER

#include <vulkan/vulkan.hpp>


namespace Ui::vkutil
{

vk::ShaderModule load_shader_module(const char* filepath, vk::Device device);

}


#endif //! GRAPHICS_RENDERER_PIPELINES_HEADER