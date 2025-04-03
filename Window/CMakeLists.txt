cmake_minimum_required(VERSION 3.31)

project(Window)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(LUD_WINDOW_SDL2 "Uses SDL2 for the translation layer" On)
option(LUD_WINDOW_SDL3 "Uses SDL3 for the translation layer" OFF)
option(LUD_WINDOW_WIN32 "Uses Win32 for the translation layer" OFF)

add_library(${PROJECT_NAME} STATIC)


set(SOURCES
	src/Window.cpp
	include/Window/Window.hpp
)


set(_translation_layer_count 0)



if(LUD_WINDOW_SDL2)
	list(APPEND SOURCES
		include/Window/SDL2/SDL2Window.hpp
		src/SDL2/SDL2Window.cpp
	)
	MATH(EXPR _translation_layer_count "${_translation_layer_count}+1")

	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static)

endif()

if(LUD_WINDOW_SDL3)
	list(APPEND SOURCES
		include/Window/SDL3/SDL3Window.hpp
		src/SDL3/SDL3Window.cpp
	)
	MATH(EXPR _translation_layer_count "${_translation_layer_count}+1")

	target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)

endif()

if(LUD_WINDOW_WIN32)
	list(APPEND SOURCES
		include/Window/WIN32/Win32Window.hpp
		src/Win32/Win32Window.cpp
	)
	MATH(EXPR _translation_layer_count "${_translation_layer_count}+1")
endif()


# if(NOT _translation_layer_count EQUAL 1)
# 	message(FATAL_ERROR "Only 1 translation layer is allowed")
# endif()


target_sources(${PROJECT_NAME} 
	PRIVATE ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC include
	PRIVATE include/Window
)

target_link_libraries(${PROJECT_NAME} 
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
