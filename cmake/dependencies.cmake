set(VULKAN_SDK $ENV{VULKAN_SDK})


set(includedir_ctre     ${CMAKE_SOURCE_DIR}/vendor/ctre/include)
set(includedir_vulkan   ${VULKAN_SDK}/Include)
set(includedir_cppicons ${CMAKE_SOURCE_DIR}/vendor/cppicons/include)
set(includedir_pfd      ${CMAKE_SOURCE_DIR}/vendor/pfd/include)

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


set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
SET(SDL2_DISABLE_INSTALL ON)
SET(SDL2_DISABLE_UNINSTALL ON)
SET(SDL_INSTALL_TESTS OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL_TEST OFF)
set(SDL2_DISABLE_SDL2MAIN ON)

FetchContent_Declare(
	SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG SDL2
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	vkbootstrap
	GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
	GIT_TAG v1.4.330
	EXCLUDE_FROM_ALL
)

include(FetchContent)
FetchContent_MakeAvailable(SDL2)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

FetchContent_MakeAvailable(vkbootstrap)
