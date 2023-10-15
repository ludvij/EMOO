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
	include "test/vendor/googletest/googletest"
	include "test/vendor/googletest/googlemock"
group ""

group "Test"
	include "test"
group ""


include "NesEmu"
