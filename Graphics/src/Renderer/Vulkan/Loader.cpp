#include "Loader.hpp"

#include "stb_image.h"

#include <iostream>
#include "Renderer/Engine.hpp"
#include "Initializers.hpp"
#include "Core.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>


namespace Ui
{


std::optional<std::vector<std::shared_ptr<MeshAsset>>> load_gltf_meshes(Engine* engine, std::filesystem::path path)
{
	fastgltf::GltfDataBuffer data;
	data.loadFromFile(path);

	constexpr auto gltf_options = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

	return std::nullopt;
}

}