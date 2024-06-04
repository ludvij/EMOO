#ifndef GRAPHICS_RENDERER_CORE_HEADER
#define GRAPHICS_RENDERER_CORE_HEADER

#include <array>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <vma/vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


//#define VK_CHECK(x) \
//do { \
//	if (err != vk::Result::eSuccess) \
//	{ \
//		std::cerr << "Detected Vulkan error: " << vk::to_string(err) << '\n'; \
//		throw std::runtime_error(vk::to_string(err)); \
//	} \
//} while (0) 
constexpr void VK_CHECK(vk::Result res)
{
	if (res != vk::Result::eSuccess)
	{
		std::cerr << "Detected Vulkan error: " << vk::to_string(res) << '\n';
		throw std::runtime_error(vk::to_string(res));
	}
}

constexpr void VK_CHECK(VkResult res)
{
	if (res != 0)
	{
		std::cerr << "Detected Vulkan error: " << string_VkResult(res) << '\n';
		throw std::runtime_error(string_VkResult(res));
	}
}

namespace Ui
{

using u32 = uint32_t;
namespace Detail
{
struct AllocatedImage
{
	vk::Image     image;
	vk::ImageView view;
	vk::Format    format{ vk::Format::eUndefined };
	vk::Extent3D  extent;
	VmaAllocation allocation{ nullptr };
};


struct AllocatedBuffer
{
	vk::Buffer        buffer;
	VmaAllocation     allocation{ nullptr };
	VmaAllocationInfo info{};

	operator vk::Buffer()
	{
		return buffer;
	}

};

struct Vertex
{
	alignas( 16 ) glm::vec3 position{};
	alignas( 8 ) glm::vec2 tex_coords{};
	int32_t textureId{ -1 };
};

struct GPUMeshBuffers
{
	AllocatedBuffer   index_buffer;
	AllocatedBuffer   vertex_buffer;
	vk::DeviceAddress address{};

};

struct GPUDrawPushConstants
{
	glm::mat4 world_matrix;
	vk::DeviceAddress vertex_buffer;
};

struct GPUSceneData
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 ambientcolor;
	glm::vec4 sunlinghtdirection;
	glm::vec4 sunlightcolor;
};
}
}
#endif