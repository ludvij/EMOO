project "NesEmu-test"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp"
	}

	flags {
		"FatalWarnings"
	}


	includedirs {
		"%{IncludeDir.ctre}",
		"%{IncludeDir.gtest}",
		"%{IncludeDir.NesEmu}",
	}
	links {
		"NesEmu"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS"
		}
	

	filter "configurations:Debug"
		defines { 
			"NES_EMU_DEBUG" 
		}
		runtime "debug"
		symbols "On"
		links {
			"%{Library.gtest_d}"
		}

	filter "configurations:Release"
		defines { 
			"NES_EMU_NDEBUG" 
		}
		links {
			"%{Library.gtest_r}"
		}
		runtime "release"
		symbols "Off"
		optimize "On"