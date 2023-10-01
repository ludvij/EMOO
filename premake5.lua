workspace "TFG"
	architecture "x86_64"
	startproject "TFG"

	configurations { 
		"Debug", 
		"Release" 
	}

	flags {
		"MultiProcessorCompile"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
group ""
	

include "TFG"
