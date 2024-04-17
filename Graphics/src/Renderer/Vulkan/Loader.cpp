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

	fastgltf::Asset gltf;
	fastgltf::Parser parser;

	if (auto load = parser.loadGltfBinary(&data, path.parent_path(), gltf_options); load)
	{
		gltf = std::move(load.get());
	}
	else
	{
		std::clog << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << '\n';
		return std::nullopt;
	}
	std::vector<std::shared_ptr<MeshAsset>> meshes;

	std::vector<uint32_t> indices;
	std::vector<Detail::Vertex> vertices;

	for (auto& mesh : gltf.meshes)
	{
		MeshAsset new_mesh;
		new_mesh.name = mesh.name;

		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives)
		{
			Detail::GeoSurface surface;
			surface.start_index = static_cast<uint32_t>(indices.size());
			surface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

			size_t initial_vtx = vertices.size();

			// load indexes
			{
				fastgltf::Accessor& index_accessor = gltf.accessors[p.indicesAccessor.value()];
				indices.reserve(indices.size() + index_accessor.count);

				fastgltf::iterateAccessor<uint32_t>(gltf, index_accessor,
					[&](uint32_t idx) {
						indices.push_back(idx + static_cast<uint32_t>(initial_vtx));
					});
			}

			// load vertex positions
			{
				fastgltf::Accessor& pos_accessor = gltf.accessors[p.findAttribute("POSITION")->second];
				vertices.resize(vertices.size() + pos_accessor.count);

				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, pos_accessor,
                    [&](glm::vec3 v, size_t index) {
                        Detail::Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = { 1, 0, 0 };
                        newvtx.color = glm::vec4 { 1.f };
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });

			}

			// load vertex normals
			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end())
			{
				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[normals->second],
					[&](glm::vec3 v, size_t index){
						vertices[initial_vtx + index].normal = v;
					});
			}

			// load vertex colors
			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end())
			{
				fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[uv->second],
					[&](glm::vec2 v, size_t index){
						vertices[initial_vtx + index].uv_x = v.x;
						vertices[initial_vtx + index].uv_y = v.y;
					});
			}

			// load colors
			auto color = p.findAttribute("COLOR_0");
			if (color != p.attributes.end())
			{
				fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[color->second],
					[&](glm::vec4 v, size_t index){
						vertices[initial_vtx + index].color = v;
					});
			}

			new_mesh.surfaces.push_back(surface);

			// display the vertex normals
		}
		constexpr bool override_colors = true;
		if (override_colors)
		{
			for (auto& vtx : vertices)
			{
				vtx.color = glm::vec4(vtx.normal, 1.0f);
			}
		}
		new_mesh.mesh_buffers = engine->UploadMesh(indices, vertices);

		meshes.emplace_back(std::make_shared<MeshAsset>(std::move(new_mesh)));

	}
	return meshes;

}

}