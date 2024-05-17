#ifndef RENDERER_VULKAN_BATCH_HEADER
#define RENDERER_VULKAN_BATCH_HEADER
#include <cstdint>

#include "Core.hpp"
#include "Renderer/RendererAPI.hpp"
#include "vkutil/Descriptors.hpp"
#include "VulkanTexture.hpp"

namespace Ui
{
class BatchRenderer
{
public:
	BatchRenderer();
	~BatchRenderer();

	void StartBatch();
	void Add(std::span<Detail::Vertex> vertices);

	void PrepareDescriptor(int binding, vkutil::DescriptorWriter& dw) const;
	void Draw(vk::CommandBuffer cmd);

	void AddTexture(VulkanBindlessTexture* texture);
	void RemoveTexture(VulkanBindlessTexture* texture);

	void Flush();

	uint32_t GetVertexCount() const
	{
		return static_cast<uint32_t>( m_vertices.size() );
	}
	uint32_t GetIndexCount() const
	{
		return static_cast<uint32_t>( m_indices.size() );
	}

	vk::DeviceAddress GetAddress() const
	{
		return m_batch_buffers.address;
	}


public:
	constexpr static size_t MAX_SPRITE_AMOUNT = 2000;
private:
	Detail::GPUMeshBuffers m_batch_buffers;

	size_t m_sprite_count{ 0 };

	std::vector<Detail::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	std::deque<uint32_t> m_ready_texture_slots;
	// texture 0 is missing texture 
	uint32_t m_texture_slots{ 0 };

	vk::DescriptorSetLayout m_descriptor;
	std::list<VulkanBindlessTexture*> m_textures;

};
}
#endif//!RENDERER_VULKAN_BATCH_HEADER