workspace "TFG"
	architecture "x86_64"
	startproject "Graphics"

	configurations { 
		"Debug", 
		"Release"
	}

	flags {
		"MultiProcessorCompile",
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


include "dependencies.lua"


group "Dependencies"
	include "vendor/premake"
	include "vendor/imgui"
	include "vendor/fastgltf"
group ""

group "utility"
	include "utility/compile_shader_to_embed"
group ""


include "Graphics"
include "NesEmu"