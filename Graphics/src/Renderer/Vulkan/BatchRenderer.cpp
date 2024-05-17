#include "BatchRenderer.hpp"
#include "Engine.hpp"
#include "vkutil/Descriptors.hpp"

#include <algorithm>
#include <utility>

#include <lud_assert.hpp>

using Ui::Detail::Vertex;

Ui::BatchRenderer::BatchRenderer()
{
	// index buffer
	m_batch_buffers.index_buffer = Engine::Get().CreateBuffer(
		MAX_SPRITE_AMOUNT * sizeof uint32_t * 6,
		vk::BufferUsageFlagBits::eIndexBuffer |
		vk::BufferUsageFlagBits::eTransferDst |
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_AUTO
	);
	m_batch_buffers.vertex_buffer = Engine::Get().CreateBuffer(
		MAX_SPRITE_AMOUNT * sizeof Vertex * 4,
		vk::BufferUsageFlagBits::eStorageBuffer |
		vk::BufferUsageFlagBits::eTransferDst |
		vk::BufferUsageFlagBits::eTransferSrc |
		vk::BufferUsageFlagBits::eShaderDeviceAddress,
		VMA_MEMORY_USAGE_AUTO
	);


	vk::BufferDeviceAddressInfo device_address_info(m_batch_buffers.vertex_buffer.buffer);
	m_batch_buffers.address = Engine::Get().GetDevice().getBufferAddress(device_address_info);
}

Ui::BatchRenderer::~BatchRenderer()
{
	Engine::Get().DestroyBuffer(m_batch_buffers.index_buffer);
	Engine::Get().DestroyBuffer(m_batch_buffers.vertex_buffer);
}

void Ui::BatchRenderer::StartBatch()
{
	if (m_sprite_count == 0)
	{
		return;
	}

	const size_t vertex_size = m_vertices.size() * sizeof Vertex;
	const size_t index_size = m_indices.size() * sizeof uint32_t;
	vmaCopyMemoryToAllocation(Engine::Get().GetAllocator(), m_vertices.data(), m_batch_buffers.vertex_buffer.allocation, 0, vertex_size);
	vmaCopyMemoryToAllocation(Engine::Get().GetAllocator(), m_indices.data(), m_batch_buffers.index_buffer.allocation, 0, index_size);

}

void Ui::BatchRenderer::Add(std::span<Detail::Vertex> vertices)
{
	Lud::check::eq(vertices.size(), 4, "Batch renderer only supports drawing quads");

	std::move(vertices.begin(), vertices.end(), std::back_inserter(m_vertices));

	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 0 ));
	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 1 ));
	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 2 ));

	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 2 ));
	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 3 ));
	m_indices.push_back(static_cast<uint32_t>( m_sprite_count * 4 + 0 ));

	m_sprite_count++;
}

void Ui::BatchRenderer::PrepareDescriptor(const int binding, vkutil::DescriptorWriter& dw) const
{
	for (const auto& t : m_textures)
	{
		dw.WriteImageBindless(binding, t->image.view, Engine::Get().m_default_sampler_nearest, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler, t->id);
	}
}

void Ui::BatchRenderer::Draw(vk::CommandBuffer cmd)
{

	StartBatch();

	cmd.bindIndexBuffer(m_batch_buffers.index_buffer, 0, vk::IndexType::eUint32);

	cmd.drawIndexed(GetIndexCount(), 1, 0, 0, 0);

	Flush();
}

void Ui::BatchRenderer::AddTexture(VulkanBindlessTexture* texture)
{
	if (m_ready_texture_slots.empty())
	{
		texture->id = m_texture_slots++;
	}
	else
	{
		texture->id = m_ready_texture_slots.front(); m_ready_texture_slots.pop_front();
	}

	m_textures.push_back(texture);

}

void Ui::BatchRenderer::RemoveTexture(VulkanBindlessTexture* texture)
{
	m_ready_texture_slots.push_back(texture->id);
	m_textures.remove(texture);
}


void Ui::BatchRenderer::Flush()
{
	m_sprite_count = 0;
	m_vertices.clear();
	m_indices.clear();
}
