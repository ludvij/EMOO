workspace "TFG"
	architecture "x86_64"
	startproject "test"

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
	include "NesEmu/vendor/googletest/googletest"
	-- include "NesEmu/vendor/googletest/googlemock"
group ""



include "NesEmu"
