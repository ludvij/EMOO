cmake_minimum_required(VERSION 3.31)

project(Window)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SOURCES
	src/Window.cpp
	src/SDL2/SDL2Window.cpp

	include/Window/SDL2/SDL2Window.hpp
	include/Window/Window.hpp
)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} 
	PRIVATE ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC include
	PRIVATE include/Window
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE SDL2::SDL2-static
	PRIVATE ludutils
	PRIVATE imgui
	PRIVATE vulkan
)

if(generator_config MATCHES "DEBUG")
	target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOW_DEBUG)
elseif(generator_config MATCHES "RELEASE")
	target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOW_NDEBUG)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOW_PLATFORM_WINDOWS)
else()
	message(SEND_ERROR "platform ${CMAKE_SYSTEM_NAME} is not supported")
endif()
