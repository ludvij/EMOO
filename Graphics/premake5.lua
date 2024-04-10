project "Graphics"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.h", 
		"src/**.cpp",
		"src/**.embed"
	}

	flags {
		"FatalWarnings"
	}

	-- pchheader "pch.hpp"
	-- pchsource "src/pch.cpp"

	includedirs {
		"src",
		"vendor/sdl2/include",
		"../vendor/ImGui",
		"%{IncludeDir.VulkanSDK}",
		"../NesEmu/src"
	}

	libdirs {
		"vendor/sdl2"
	}

	links {
		"SDL2",
		"ImGui",
		"NesEmu",
		"%{Library.Vulkan}"
	}

	defines {
		"SDL_MAIN_HANDLED"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS",
			"WIN32"
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
		optimize "On"

include "setupVulkan.lua"
	