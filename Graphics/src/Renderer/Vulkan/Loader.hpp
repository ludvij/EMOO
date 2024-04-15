#ifndef GRAPHICS_RENDERER_LOADER
#define GRAPHICS_RENDERER_LOADER
#include "Core.hpp"
#include <unordered_map>

#include <lud_slurper.hpp>

namespace Ui
{
namespace Detail
{
struct GeoSurface
{
	uint32_t start_index;
	uint32_t count;
};
}

struct MeshAsset
{
	std::string name;

	std::vector<Detail::GeoSurface> surfaces;
	Detail::GPUMeshBuffers mesh_buffers;
};

class Engine;

std::optional<std::vector<std::shared_ptr<MeshAsset>>> load_gltf_meshes(Engine* engine, std::filesystem::path path);
}

#endif //! GRAPHICS_RENDERER_LOADER