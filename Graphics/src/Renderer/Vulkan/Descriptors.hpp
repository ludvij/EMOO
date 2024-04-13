#ifndef GRAPHICS_RENDERER_DESCRIPTORS_HEADER
#define GRAPHICS_RENDERER_DESCRIPTORS_HEADER

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Ui::Detail
{
struct DescriptorLayoutBuilder
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings;

	void add_binding(uint32_t binding, vk::DescriptorType type);
	void clear();
	vk::DescriptorSetLayout build(vk::Device device, vk::ShaderStageFlags shader_stages);
};

struct DescriptorAllocator
{
	struct PoolSizeRatio
	{
		vk::DescriptorType type;
		float ratio{};
	};


	vk::DescriptorPool pool;

	void init_pool(vk::Device device, uint32_t max_sets, std::span<PoolSizeRatio> pool_ratios);
	void clear_descriptors(vk::Device device);
	void destroy_pool(vk::Device device);

	vk::DescriptorSet allocate(vk::Device device, vk::DescriptorSetLayout layout);
};
}

#endif