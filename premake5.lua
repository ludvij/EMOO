workspace "TFG"
	architecture "x86_64"
	startproject "NesEmu"

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
	include "tests/vendor/googletest1140/googletest"
	include "tests/vendor/googletest1140/googlemock"
group ""

group "Test"
	include "tests"
group ""


include "NesEmu"
