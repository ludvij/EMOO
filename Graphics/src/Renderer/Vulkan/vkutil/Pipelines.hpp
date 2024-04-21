#ifndef GRAPHICS_RENDERER_UTIL_PIPELINES_HEADER
#define GRAPHICS_RENDERER_UTIL_PIPELINES_HEADER

#include <vulkan/vulkan.hpp>

#include <vector>


namespace Ui::vkutil
{

vk::ShaderModule load_shader_module(const char* filepath, vk::Device device);
vk::ShaderModule load_shader_module(const std::span<uint32_t> code, vk::Device device);

class PipelineBuilder
{
public:
	PipelineBuilder();

	vk::Pipeline Build(vk::Device device);
	PipelineBuilder& Clear();

	PipelineBuilder& SetPipelineLayout(vk::PipelineLayout layout);
	PipelineBuilder& SetShaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader);
	PipelineBuilder& SetInputTopology(vk::PrimitiveTopology topology);
	PipelineBuilder& SetPolygonMode(vk::PolygonMode polygon_mode);
	PipelineBuilder& SetCullMode(vk::CullModeFlags cull_mode, vk::FrontFace front_face);
	PipelineBuilder& SetMultisamplingNone();
	PipelineBuilder& DisableBlending();
	PipelineBuilder& EnableBlendingAdditive();
	PipelineBuilder& EnableBlendingAlphablend();
	PipelineBuilder& SetColorAttachmentFormat(vk::Format format);
	PipelineBuilder& SetDepthFormat(vk::Format format);
	PipelineBuilder& DisableDepthTest();
	PipelineBuilder& EnableDepthTest(bool depth_write_enable, vk::CompareOp op);

private:
	std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stages;
	vk::PipelineInputAssemblyStateCreateInfo       m_input_assembly;
	vk::PipelineRasterizationStateCreateInfo       m_rasterizer;
	vk::PipelineColorBlendAttachmentState          m_color_blend_attachment;
	vk::PipelineMultisampleStateCreateInfo         m_multisampling;
	vk::PipelineLayout                             m_pipeline_layout;
	vk::PipelineDepthStencilStateCreateInfo        m_depth_stencil;
	vk::PipelineRenderingCreateInfo                m_render_info;
	vk::Format                                     m_color_attachment_format;

};

}


#endif //! GRAPHICS_RENDERER_UTIL_PIPELINES_HEADER