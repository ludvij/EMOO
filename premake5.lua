include "dependencies.lua"

workspace "TFG"	
	architecture "x86_64"
	startproject "Application"

	configurations { 
		"Debug", 
		"Release"
	}

	ignoredefaultlibraries {
		"MSVCRT",
		"MSVCRTd"
	}

	flags {
		"MultiProcessorCompile",
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
sdl3 = false



group "Dependencies"
	include "vendor/premake"
	include "vendor/imgui"
group ""

group "Test"
	include "NesEmu/test"
group ""


group "Interface"
	include "Application"
	include "Window"
	include "Input"
	include "Renderer"
group ""
include "NesEmu"