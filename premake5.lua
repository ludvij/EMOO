outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
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




group "Dependencies"
	-- include "vendor/premake"
	include "vendor/imgui"
	include "vendor/googletest/googletest"
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

group "Utility"
	include "FileManager"
group ""
include "NesEmu"