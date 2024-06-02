project "Input"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp",
	}

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"%{IncludeDir.sdl3}",
		"%{IncludeDir.lud_utils}",

	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS"
		}
	
	postbuildcommands {
		-- "{COPYDIR} %[palettes] %[../Graphics/Palettes]"
	}
	
	

	filter "configurations:Debug"
		defines { 
			"INPUT_DEBUG" 
		}
		runtime "debug"
		symbols "On"

	filter "configurations:Release"
		defines { 
			"INPUT_NDEBUG" 
		}
		runtime "release"
		symbols "Off"
		optimize "On"

