workspace "TFG"
	architecture "x86_64"
	startproject "Graphics"

	configurations { 
		"Debug", 
		"Release"
	}

	flags {
		"MultiProcessorCompile",
		"FatalWarnings"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
group ""



include "NesEmu"
include "GraphicsExternal.lua"