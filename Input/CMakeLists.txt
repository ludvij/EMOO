cmake_minimum_required(VERSION 3.31)



project(Input)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SOURCES
	src/Input.cpp
	src/SDL2/SDL2Input.cpp

	include/Input/SDL2/SDL2Input.hpp
	include/Input/Input.hpp
)

add_library(${PROJECT_NAME} STATIC)


target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_EXPORT_COMPILE_COMMANDS=1)


target_sources(${PROJECT_NAME} 
	PRIVATE ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
	INTERFACE include
	PRIVATE include/Input
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE SDL2::SDL2-static
	PRIVATE ludutils
)

if(generator_config MATCHES "DEBUG")
	target_compile_definitions(${PROJECT_NAME} PRIVATE INPUT_DEBUG)
elseif(generator_config MATCHES "RELEASE")
	target_compile_definitions(${PROJECT_NAME} PRIVATE INPUT_NDEBUG)
endif()
