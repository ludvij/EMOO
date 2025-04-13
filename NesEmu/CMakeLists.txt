cmake_minimum_required(VERSION 3.31)

project(NesEmu)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
	src/Console.hpp
	src/Console.cpp
	src/NesEmu.hpp
	
	src/internals/Bus.hpp
	src/internals/Bus.cpp
	src/internals/Cartridge.hpp
	src/internals/Cartridge.cpp
	src/internals/Core.hpp
	src/internals/CPU.hpp
	src/internals/CPU.cpp
	src/internals/APU.hpp
	src/internals/APU.cpp
	src/internals/InputDevice.hpp
	src/internals/InputDevice.cpp
	src/internals/PPU.hpp
	src/internals/PPU.cpp

	src/internals/mappers/Mapper.hpp
	src/internals/mappers/MapperAlias.hpp
	src/internals/mappers/MMC1.hpp
	src/internals/mappers/MMC1.cpp
	src/internals/mappers/NROM.hpp
	src/internals/mappers/NROM.cpp

	src/utils/A6502Core.hpp
	src/utils/A6502Core.cpp
	src/utils/Assembler.hpp
	src/utils/Assembler.cpp
	src/utils/Disassembler.hpp
	src/utils/Disassembler.cpp
	src/utils/Unreachable.hpp

	src/pch.hpp
)

set(nemu_tests_dir ${CMAKE_CURRENT_LIST_DIR}/test/src)

file(GLOB_RECURSE tests_sources ${nemu_tests_dir}/*)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} 
	PRIVATE ${SOURCES}
)

target_precompile_headers(${PROJECT_NAME}
	PRIVATE src/pch.hpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC src
	PRIVATE ${includedir_ctre}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC ludutils
	PUBLIC Fman
)

include(FetchContent)

FetchContent_Declare(
	gtest
	GIT_REPOSITORY git@github.com:google/googletest.git
	GIT_TAG v1.16.0
	EXCLUDE_FROM_ALL
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

add_executable(Test-NesEmu EXCLUDE_FROM_ALL)
target_link_libraries(Test-NesEmu 
	PRIVATE GTest::gtest
	PRIVATE NesEmu
)
target_sources(Test-NesEmu PRIVATE ${tests_sources})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_compile_definitions(${PROJECT_NAME} PRIVATE NES_EMU_PLATFORM_WINDOWS)
endif()

if(generator_config MATCHES "DEBUG")
	target_compile_definitions(${PROJECT_NAME} PRIVATE NES_EMU_DEBUG)
elseif(generator_config MATCHES "RELEASE")
	target_compile_definitions(${PROJECT_NAME} PRIVATE NES_EMU_NDEBUG)
endif()
