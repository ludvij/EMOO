cmake_minimum_required(VERSION 3.31)

project(Application)

set(SOURCES
	src/main.cpp
	src/Application.hpp
	src/Application.cpp

	src/Components/IComponent.hpp
	src/Components/IComponent.cpp
	src/Components/CloseDialog.hpp
	src/Components/CloseDialog.cpp
	src/Components/MemoryView.hpp
	src/Components/MemoryView.cpp
	src/Components/ShowCpuStatus.hpp
	src/Components/ShowCpuStatus.cpp
	src/Components/ShowPPUStatus.hpp
	src/Components/ShowPPUStatus.cpp

	src/ImGui/ImGuiBuild.cpp

	src/embedding/explanations.hpp

	resources/resource.rc
	resources/texts/explanations.txf

	resources/fonts/CascadiaMono-Regular.embed
	resources/fonts/fa-solid-900.embed
	resources/fonts/OpenSans-Regular.embed
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME EMOO)

target_include_directories(${PROJECT_NAME}
	PRIVATE src
	PRIVATE resources
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE SDL_MAIN_HANDLED
	PRIVATE IMGUI_DEFINE_MATH_OPERATORS
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ludutils
	PRIVATE SDL2::SDL2-static
	PRIVATE Window
	PRIVATE Input
	PRIVATE imgui
	PRIVATE NesEmu
	PRIVATE Renderer
	PRIVATE Fman
	PRIVATE cppicons
	PRIVATE pfd
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_LIST_DIR}/src/embedding/explanations.hpp
	COMMAND py ${CMAKE_CURRENT_LIST_DIR}/scripts/make_ex_file.py ${CMAKE_CURRENT_LIST_DIR}/resources/texts/explanations.txf ${CMAKE_CURRENT_LIST_DIR}/src/embedding/explanations.hpp
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/resources/texts/explanations.txf
	CODEGEN
	VERBATIM
)