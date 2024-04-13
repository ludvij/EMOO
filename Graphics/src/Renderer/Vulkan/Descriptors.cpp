#include "Descriptors.hpp"
#include "Renderer/Engine.hpp"

namespace Ui::Detail
{
void DescriptorLayoutBuilder::add_binding(uint32_t binding, vk::DescriptorType type)
{
	vk::DescriptorSetLayoutBinding newbind(binding, type, 1);

	bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::clear()
{
	bindings.clear();
}

vk::DescriptorSetLayout DescriptorLayoutBuilder::build(vk::Device device, vk::ShaderStageFlags shaders_tages)
{
	for (auto& b : bindings)
	{
		b.stageFlags |= shaders_tages;
	}

	vk::DescriptorSetLayoutCreateInfo info({}, bindings);

	vk::DescriptorSetLayout set = device.createDescriptorSetLayout(info);
	
	return set;
}

void DescriptorAllocator::init_pool(vk::Device device, uint32_t max_sets, std::span<PoolSizeRatio> pool_ratios)
{
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	for (const auto& ratio : pool_ratios)
	{
		pool_sizes.push_back(vk::DescriptorPoolSize(ratio.type, static_cast<uint32_t>(ratio.ratio * max_sets)));
	}

	vk::DescriptorPoolCreateInfo pool_info({}, max_sets, pool_sizes);

	pool = device.createDescriptorPool(pool_info);
}

void DescriptorAllocator::clear_descriptors(vk::Device device)
{
	device.resetDescriptorPool(pool, {});
}

void DescriptorAllocator::destroy_pool(vk::Device device)
{
	device.destroyDescriptorPool(pool);
}

vk::DescriptorSet DescriptorAllocator::allocate(vk::Device device, vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo alloc_info(pool, layout);

	vk::DescriptorSet ds = device.allocateDescriptorSets(alloc_info).front();

	return ds;
}
}