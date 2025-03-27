project "Window"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")


	files { 
		"include/Window/*.hpp", 
		"src/*.cpp",
		"include/Window/SDL2/**.hpp",
		"src/SDL2/**.cpp"
	}

	fatalwarnings { "All" }

	includedirs {
		"src",
		"include/Window/",
		"%{IncludeDir.sdl}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.imgui}",

	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"WINDOW_PLATFORM_WINDOWS"
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

