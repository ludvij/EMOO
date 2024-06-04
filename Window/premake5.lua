project "Window"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"include/**.hpp", 
		"src/**.cpp",
	}

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"include/Window/",
		"%{IncludeDir.sdl3}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.imgui}",

	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"WINDOW_PLATFORM_WINDOWS"
		}
	
	postbuildcommands {
		-- "{COPYDIR} %[palettes] %[../Graphics/Palettes]"
	}
	
	

	filter "configurations:Debug"
		defines { 
			"WINDOW_DEBUG" 
		}
		runtime "debug"
		symbols "On"

	filter "configurations:Release"
		defines { 
			"WINDOW_NDEBUG" 
		}
		runtime "release"
		symbols "Off"
		optimize "On"

