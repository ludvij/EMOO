#include "Pipelines.hpp"

#define LUD_SLURPER_IMPLEMENTATION
#include <lud_slurper.hpp>

#include "../Initializers.hpp"

namespace Ui::vkutil
{
vk::ShaderModule load_shader_module(const char* filepath, vk::Device device)
{
	auto code = Lud::Slurper::SlurpTo<uint32_t>(filepath, std::ios::binary);

	vk::ShaderModuleCreateInfo info({}, code);

	vk::ShaderModule module = device.createShaderModule(info);

	return module;
}

PipelineBuilder::PipelineBuilder()
{
	Clear();
}
PipelineBuilder& PipelineBuilder::Clear()
{
	m_input_assembly          = vk::PipelineInputAssemblyStateCreateInfo();
	m_rasterizer              = vk::PipelineRasterizationStateCreateInfo();
	m_color_blend_attachment  = vk::PipelineColorBlendAttachmentState();
	m_multisampling           = vk::PipelineMultisampleStateCreateInfo();
	m_pipeline_layout         = vk::PipelineLayout();
	m_depth_stencil           = vk::PipelineDepthStencilStateCreateInfo();
	m_render_info             = vk::PipelineRenderingCreateInfo();
	m_color_attachment_format = vk::Format();

	m_shader_stages.clear();

	return *this;
}

PipelineBuilder& PipelineBuilder::SetPipelineLayout(vk::PipelineLayout layout)
{
	m_pipeline_layout = layout;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetShaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader)
{
	m_shader_stages.clear();

	m_shader_stages.push_back(vkinit::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eVertex, vertex_shader));
	m_shader_stages.push_back(vkinit::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eFragment, fragment_shader));

	return *this;
}

PipelineBuilder& PipelineBuilder::SetInputTopology(vk::PrimitiveTopology topology)
{
	m_input_assembly.topology = topology;

	m_input_assembly.primitiveRestartEnable = vk::False;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetPolygonMode(vk::PolygonMode polygon_mode)
{
	m_rasterizer.polygonMode = polygon_mode;
	m_rasterizer.lineWidth = 1;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetCullMode(vk::CullModeFlags cull_mode, vk::FrontFace front_face)
{
	m_rasterizer.cullMode = cull_mode;
	m_rasterizer.frontFace = front_face;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetMultisamplingNone()
{
	m_multisampling.sampleShadingEnable = vk::False;
	m_multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	m_multisampling.minSampleShading = 1.0f;
	m_multisampling.pSampleMask = nullptr;
	m_multisampling.alphaToCoverageEnable = vk::False;
	m_multisampling.alphaToOneEnable = vk::False;

	return *this;
}

PipelineBuilder& PipelineBuilder::DisableBlending()
{
	m_color_blend_attachment.colorWriteMask = 
		vk::ColorComponentFlagBits::eR | 
		vk::ColorComponentFlagBits::eG | 
		vk::ColorComponentFlagBits::eB | 
		vk::ColorComponentFlagBits::eA;
	m_color_blend_attachment.blendEnable = vk::False;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetColorAttachmentFormat(vk::Format format)
{
	m_color_attachment_format = format;
	m_render_info.colorAttachmentCount = 1;
	m_render_info.pColorAttachmentFormats = &m_color_attachment_format;
	
	return *this;
}

PipelineBuilder& PipelineBuilder::SetDepthFormat(vk::Format format)
{
	m_render_info.depthAttachmentFormat = format;

	return *this;
}

PipelineBuilder& PipelineBuilder::DisableDepthTest()
{
	m_depth_stencil.depthTestEnable = vk::False;
	m_depth_stencil.depthWriteEnable = vk::False;
	m_depth_stencil.depthCompareOp = vk::CompareOp::eNever;
	m_depth_stencil.depthBoundsTestEnable = vk::False;
	//m_depth_stencil.front = {};
	//m_depth_stencil.back = {};
	m_depth_stencil.minDepthBounds = 0.0f;
	m_depth_stencil.maxDepthBounds = 1.0f;

	return *this;
}



vk::Pipeline PipelineBuilder::Build(vk::Device device)
{
	vk::PipelineViewportStateCreateInfo viewport_state;
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	vk::PipelineColorBlendStateCreateInfo color_blending;
	color_blending.logicOpEnable = vk::False;
	color_blending.logicOp = vk::LogicOp::eCopy;
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &m_color_blend_attachment;

	vk::PipelineVertexInputStateCreateInfo vertex_input_info;

	vk::GraphicsPipelineCreateInfo pipeline_info;
	pipeline_info.pNext = &m_render_info;

	pipeline_info.setStages(m_shader_stages);
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &m_input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &m_rasterizer;
	pipeline_info.pMultisampleState = &m_multisampling;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDepthStencilState = &m_depth_stencil;
	pipeline_info.layout = m_pipeline_layout;

	vk::DynamicState state[] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic_info({}, state);

	pipeline_info.pDynamicState = &dynamic_info;

	auto [res, pipeline] = device.createGraphicsPipeline({}, pipeline_info);
	if (res != vk::Result::eSuccess)
	{
		throw std::runtime_error("Couldn't create pipeline");
	}
	return pipeline;
}


}