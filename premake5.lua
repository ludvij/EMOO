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
	include "vendor/imgui"
group ""



group "Core"
	include "Graphics"
	include "NesEmu"
group ""