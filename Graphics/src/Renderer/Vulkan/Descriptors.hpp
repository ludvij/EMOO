#ifndef GRAPHICS_RENDERER_DESCRIPTORS_HEADER
#define GRAPHICS_RENDERER_DESCRIPTORS_HEADER

#include <deque>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Ui::Detail
{
class DescriptorLayoutBuilder
{
public:
	DescriptorLayoutBuilder& AddBinding(uint32_t binding, vk::DescriptorType type);
	void Clear();
	vk::DescriptorSetLayout Build(vk::Device device, vk::ShaderStageFlags shader_stages);
private:
	std::vector<vk::DescriptorSetLayoutBinding> bindings;
};

class DescriptorAllocatorGrowable
{
public:
	struct PoolSizeRatio
	{
		vk::DescriptorType type;
		float ratio{};
	};

	void Init(vk::Device device, uint32_t inital_sets, std::span<PoolSizeRatio> pool_ratios);
	void ClearPools(vk::Device device);
	void DestroyPools(vk::Device device);

	vk::DescriptorSet Allocate(vk::Device device, vk::DescriptorSetLayout layout);

private:
	vk::DescriptorPool get_pool(vk::Device device);
	vk::DescriptorPool create_pool(vk::Device device, uint32_t set_count, std::span<PoolSizeRatio> pool_ratios);

private:
	std::vector<PoolSizeRatio>      ratios;
	std::vector<vk::DescriptorPool> full_pools;
	std::vector<vk::DescriptorPool> ready_pools;
	uint32_t sets_per_pool{ 0 };
};

class DescriptorWriter
{
public:

	DescriptorWriter& WriteImage(int binding, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type);
	DescriptorWriter& WriteBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type);

	void Clear();
	void UpdateSet(vk::Device device, vk::DescriptorSet set);

private:
	std::deque<vk::DescriptorImageInfo>  image_infos;
	std::deque<vk::DescriptorBufferInfo> buffer_infos;
	std::vector<vk::WriteDescriptorSet>  writes;
};
}

#endif