cmake_minimum_required(VERSION 3.31)

project(Renderer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SOURCES
	src/Primitives.hpp
	src/RendererAPI.hpp
	src/Sprite.hpp
	src/Sprite.cpp
	src/Texture.hpp
	src/Texture.cpp

	src/Vulkan/BatchRenderer.hpp
	src/Vulkan/BatchRenderer.cpp
	src/Vulkan/Core.hpp
	src/Vulkan/Engine.hpp
	src/Vulkan/Engine.cpp
	src/Vulkan/RendererAPIVulkanImpl.cpp
	src/Vulkan/VulkanTexture.hpp
	src/Vulkan/VulkanTexture.cpp

	src/Vulkan/vkutil/Descriptors.hpp
	src/Vulkan/vkutil/Descriptors.cpp
	src/Vulkan/vkutil/Images.hpp
	src/Vulkan/vkutil/Images.cpp
	src/Vulkan/vkutil/Initializers.hpp
	src/Vulkan/vkutil/Initializers.cpp
	src/Vulkan/vkutil/Pipelines.hpp
	src/Vulkan/vkutil/Pipelines.cpp
	src/Vulkan/vkutil/Types.hpp

	Shader/embed/mesh.frag.cpp
	Shader/embed/mesh.vert.cpp

)


add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} 
	PRIVATE ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC src
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC vulkan
	PRIVATE ludutils
	PRIVATE imgui
	PRIVATE Window
	PRIVATE vk-bootstrap
)

if(generator_config MATCHES "DEBUG")
	target_compile_definitions(${PROJECT_NAME} 
		PRIVATE GRAPHICS_DEBUG
		PRIVATE LUD_TRACE_ID_LIFETIMES
	)
elseif(generator_config MATCHES "RELEASE")
	target_compile_definitions(${PROJECT_NAME} PRIVATE GRAPHICS_NDEBUG)
endif()

target_compile_definitions(${PROJECT_NAME} 
	PRIVATE GLM_ENABLE_EXPERIMENTAL
	PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE
)

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_compile_definitions(${PROJECT_NAME} 
		PRIVATE RENDERER_PLATFORM_WINDOWS
		PRIVATE WIN32
		PRIVATE GRAPHICS_SHOW_FPS
	)
else()
	message(SEND_ERROR "platform ${CMAKE_SYSTEM_NAME} is not supported")
endif()

set(SHADER_TYPES
	mesh.frag
	mesh.vert
)
foreach(shader_t ${SHADER_TYPES})
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_LIST_DIR}/Shader/embed/${shader_t}.cpp
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/Shader/embed
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/Shader/SPIRV
		COMMAND ${VULKAN_SDK}/BIN/glslangValidator -V -o ${CMAKE_CURRENT_LIST_DIR}/Shader/SPIRV/${shader_t}.spv ${CMAKE_CURRENT_LIST_DIR}/Shader/${shader_t}
		COMMAND py ${CMAKE_CURRENT_LIST_DIR}/scripts/spv_to_embed.py ${CMAKE_CURRENT_LIST_DIR}/Shader/SPIRV/${shader_t}.spv ${CMAKE_CURRENT_LIST_DIR}/Shader/embed/${shader_t}.cpp
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/Shader/${shader_t}
		CODEGEN
		VERBATIM
	)
	
endforeach(shader_t)

