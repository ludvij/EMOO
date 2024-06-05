project "Window"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"include/Window/*.hpp", 
		"src/*.cpp",
	}
	if (sdl3) then
		files {
			"include/Window/SDL3/**.hpp",
			"src/SDL3/**.cpp"
		}
	else
		files {
			"include/Window/SDL2/**.hpp",
			"src/SDL2/**.cpp"
		}
	end

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"include/Window/",
		sdl3 and "%{IncludeDir.sdl3}" or "%{IncludeDir.sdl2}",
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

