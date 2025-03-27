project "NesEmu-test"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++23"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp"
	}

	fatalwarnings { "All" }


	includedirs {
		"%{IncludeDir.ctre}",
		"%{IncludeDir.gtest}",
		"%{IncludeDir.NesEmu}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.FileManager}",
	}
	links {
		"NesEmu",
		"FileManager",
		"googletest"
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

	filter "configurations:Release"
		defines { 
			"NES_EMU_NDEBUG" 
		}
		runtime "release"
		symbols "Off"
		optimize "On"