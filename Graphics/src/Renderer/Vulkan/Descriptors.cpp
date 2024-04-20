#include "Descriptors.hpp"
#include "Renderer/Engine.hpp"

namespace Ui::Detail
{
DescriptorLayoutBuilder& DescriptorLayoutBuilder::AddBinding(uint32_t binding, vk::DescriptorType type)
{
	vk::DescriptorSetLayoutBinding newbind(binding, type, 1);

	bindings.push_back(newbind);

	return *this;
}

void DescriptorLayoutBuilder::Clear()
{
	bindings.clear();
}

vk::DescriptorSetLayout DescriptorLayoutBuilder::Build(vk::Device device, vk::ShaderStageFlags shaders_tages)
{
	for (auto& b : bindings)
	{
		b.stageFlags |= shaders_tages;
	}

	vk::DescriptorSetLayoutCreateInfo info({}, bindings);

	vk::DescriptorSetLayout set = device.createDescriptorSetLayout(info);

	return set;
}

void DescriptorAllocatorGrowable::Init(vk::Device device, uint32_t initial_sets, std::span<PoolSizeRatio> pool_ratios)
{
	ratios.clear();

	for (const auto& r : pool_ratios)
	{
		ratios.push_back(r);
	}

	auto new_pool = create_pool(device, initial_sets, pool_ratios);

	sets_per_pool = static_cast<uint32_t>( initial_sets * 1.5 );

	ready_pools.push_back(new_pool);
}
void DescriptorAllocatorGrowable::ClearPools(vk::Device device)
{
	for (const auto& p : ready_pools)
	{
		device.resetDescriptorPool(p);
	}
	for (const auto& p : full_pools)
	{
		device.resetDescriptorPool(p);
		ready_pools.push_back(p);
	}
	full_pools.clear();
}
void DescriptorAllocatorGrowable::DestroyPools(vk::Device device)
{
	for (const auto& p : ready_pools)
	{
		device.destroyDescriptorPool(p);
	}
	ready_pools.clear();
	for (const auto& p : full_pools)
	{
		device.destroyDescriptorPool(p);
	}
	full_pools.clear();
}
vk::DescriptorSet DescriptorAllocatorGrowable::Allocate(vk::Device device, vk::DescriptorSetLayout layout)
{
	auto pool_to_use = get_pool(device);

	vk::DescriptorSetAllocateInfo alloc_info(pool_to_use, layout);

	vk::DescriptorSet ds;

	bool retry = false;
	try
	{
		ds = device.allocateDescriptorSets(alloc_info).front();
	} catch (const vk::OutOfPoolMemoryError&)
	{
		retry = true;
	} catch (const vk::FragmentedPoolError&)
	{
		retry = true;
	}

	if (retry)
	{
		full_pools.push_back(pool_to_use);

		pool_to_use = get_pool(device);
		alloc_info.descriptorPool = pool_to_use;
		ds = device.allocateDescriptorSets(alloc_info).front();
	}

	ready_pools.push_back(pool_to_use);

	return ds;

}
vk::DescriptorPool DescriptorAllocatorGrowable::get_pool(vk::Device device)
{
	vk::DescriptorPool new_pool;
	if (!ready_pools.empty())
	{
		new_pool = ready_pools.back();
		ready_pools.pop_back();
	}
	else
	{
		// create new pool
		new_pool = create_pool(device, sets_per_pool, ratios);
		sets_per_pool = static_cast<uint32_t>( sets_per_pool * 1.5 );
		if (sets_per_pool > 4092)
		{
			sets_per_pool = 4029;
		}
	}

	return new_pool;
}
vk::DescriptorPool DescriptorAllocatorGrowable::create_pool(vk::Device device, uint32_t set_count, std::span<PoolSizeRatio> pool_ratios)
{
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	for (const auto& ratio : pool_ratios)
	{
		pool_sizes.push_back(vk::DescriptorPoolSize(ratio.type, static_cast<uint32_t>( ratio.ratio * set_count )));
	}

	vk::DescriptorPoolCreateInfo pool_info({}, set_count, pool_sizes);

	vk::DescriptorPool new_pool = device.createDescriptorPool(pool_info);


	return new_pool;
}
DescriptorWriter& DescriptorWriter::WriteImage(int binding, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type)
{
	vk::DescriptorImageInfo& info = image_infos.emplace_back(vk::DescriptorImageInfo{ sampler, view, layout });

	vk::WriteDescriptorSet write;

	write.dstBinding = binding;
	write.dstSet = nullptr;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pImageInfo = &info;

	writes.push_back(write);

	return *this;

}
DescriptorWriter& DescriptorWriter::WriteBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type)
{
	vk::DescriptorBufferInfo& info = buffer_infos.emplace_back(vk::DescriptorBufferInfo{ buffer, offset, size });

	vk::WriteDescriptorSet write;
	write.dstBinding = binding;
	write.dstSet = nullptr;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = &info;

	writes.push_back(write);

	return *this;
}
void DescriptorWriter::Clear()
{
	image_infos.clear();
	buffer_infos.clear();
	writes.clear();
}
void DescriptorWriter::UpdateSet(vk::Device device, vk::DescriptorSet set)
{
	for (auto& write : writes)
	{
		write.dstSet = set;
	}

	device.updateDescriptorSets(writes, {});
}
}