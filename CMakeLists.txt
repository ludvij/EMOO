cmake_minimum_required(VERSION 3.31)
project(EMOO)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)



set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)


set(generator_config 
	$<$<CONFIG:Debug>:DEBUG>
	$<$<CONFIG:Release,MinSizeRel,RelWithDebugInfo>:RELEASE>
)


if(MSVC)
	SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# SET(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()


set (bin_dir "${CMAKE_SOURCE_DIR}/bin")
set (output_dir "${bin_dir}/${CMAKE_SYSTEM_NAME}-$<CONFIG>")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir}/lib)


# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# if (generator_config MATCHES DEBUG)
# 	if(MSVC)
# 		add_compile_options(/fsanitize=address)
# 		add_link_options(/fsanitize=address)
# 	else()
# 		add_compile_options(-fsanitize=address)
# 		add_link_options(-fsanitize=address)
# 	endif()
# endif()

# EXTERNAL CODE =========================================================
include(dependencies.cmake)

add_subdirectory(vendor/imgui)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
set(SDL2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/vendor/sdl2)
add_subdirectory(vendor/sdl2 EXCLUDE_FROM_ALL)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/googletest EXCLUDE_FROM_ALL)

set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL TRUE)
#!EXTERNAL CODE =========================================================


add_subdirectory(Lud_Utils)
set(LUD_WINDOW_SDL2 ON)
add_subdirectory(Window)
add_subdirectory(Input)
add_subdirectory(Renderer)
add_subdirectory(FileManager)
add_subdirectory(NesEmu)
add_subdirectory(Application)



set_target_properties(SDL2-static      PROPERTIES FOLDER "Dependencies")
set_target_properties(sdl_headers_copy PROPERTIES FOLDER "Dependencies")
set_target_properties(imgui            PROPERTIES FOLDER "Dependencies")
set_target_properties(vkbootstrap      PROPERTIES FOLDER "Dependencies")
set_target_properties(gtest            PROPERTIES FOLDER "Dependencies")
set_target_properties(gtest_main       PROPERTIES FOLDER "Dependencies")

set_target_properties(Application      PROPERTIES FOLDER "Application")
set_target_properties(Window           PROPERTIES FOLDER "Application")
set_target_properties(Renderer         PROPERTIES FOLDER "Application")
set_target_properties(Input            PROPERTIES FOLDER "Application")

set_target_properties(Fman             PROPERTIES FOLDER "Utility")
set_target_properties(ludutils         PROPERTIES FOLDER "Utility")

set_target_properties(Test-NesEmu      PROPERTIES FOLDER "Tests")