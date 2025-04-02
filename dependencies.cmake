set(VULKAN_SDK $ENV{VULKAN_SDK})

set(includedir_ctre     ${CMAKE_CURRENT_LIST_DIR}/vendor/ctre/include)
set(includedir_vulkan   ${VULKAN_SDK}/Include)
set(includedir_cppicons ${CMAKE_CURRENT_LIST_DIR}/vendor/cppicons/include)
set(includedir_pfd      ${CMAKE_CURRENT_LIST_DIR}/vendor/pfd/include)
set(includedir_vkb      ${CMAKE_CURRENT_LIST_DIR}/vendor/vk_bootstrap)

set(librarydir_vulkan ${VULKAN_SDK}/Lib)

set(library_vulkan ${librarydir_vulkan}/vulkan-1.lib)


add_library(vulkan STATIC IMPORTED)
set_target_properties(vulkan PROPERTIES
	IMPORTED_LOCATION ${library_vulkan}
	INTERFACE_INCLUDE_DIRECTORIES ${includedir_vulkan}
)

add_library(pfd INTERFACE)
target_include_directories(pfd
	INTERFACE ${includedir_pfd}
)

add_library(cppicons INTERFACE)
target_include_directories(cppicons
	INTERFACE ${includedir_cppicons}
)

add_library(vkbootstrap STATIC)
target_sources(vkbootstrap PRIVATE 
	${includedir_vkb}/VkBootstrap.cpp
	${includedir_vkb}/VkBootstrap.h
	${includedir_vkb}/VkBootstrapDispatch.h
)
target_include_directories(vkbootstrap
	PUBLIC ${includedir_vkb}
)
target_link_libraries(vkbootstrap PRIVATE vulkan)