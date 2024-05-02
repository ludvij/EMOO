#ifndef RENDERER_VULKAN_BATCH_HEADER
#define RENDERER_VULKAN_BATCH_HEADER
#include <cstdint>

#include "Core.hpp"
#include "Renderer/RendererAPI.hpp"

namespace Ui
{
class BatchRenderer
{
public:
	BatchRenderer();
	~BatchRenderer();

	void StartBatch();
	void Add(std::span<Detail::Vertex> vertices);
	void Draw(vk::CommandBuffer cmd);

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

private:

public:
	constexpr static size_t MAX_SPRITE_AMOUNT = 2000;
	constexpr static float GROWTH_FACTOR = 1.50f;
private:
	Detail::GPUMeshBuffers m_batch_buffers;

	size_t m_sprite_count{ 0 };

	std::vector<Detail::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

};
}
#endif//!RENDERER_VULKAN_BATCH_HEADER