project "Graphics"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.h", 
		"src/**.cpp",
		"src/**.embed",
		"Shader/**.comp",
		"Shader/**.frag",
		"Shader/**.vert",
	}

	flags {
		"FatalWarnings"
	}

	-- pchheader "pch.hpp"
	-- pchsource "src/pch.cpp"

	includedirs {
		"src",
		"%{IncludeDir.sdl2}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.NesEmu}",
		"%{IncludeDir.tiny_gltf}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.json}",
		"%{IncludeDir.lud_utils}",

	}

	libdirs {
		"%{LibraryDir.sdl}"
	}

	links {
		"ImGui",
		"NesEmu",
		"%{Library.sdl2}",
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
			"GRAPHICS_DEBUG" 
		}
		runtime "debug"
		symbols "On"
	
	filter "configurations:Release"
		defines { 
			"GRAPHICS_NDEBUG",
			"NDEBUG"
		}

		runtime "release"
		optimize "On"

include "dependencies.lua"

	filter 'files:Shader/*'
		buildmessage "Compiling %{file.relpath}"
		buildcommands {
			'%{VULKAN_SDK}/BIN/glslangValidator -V -o "%{file.directory}/SPIRV/%{file.name}.spv" "%{file.relpath}"'
		}

		buildoutputs {
			"%{file.directory}/SPIRV/%{file.basename}.spv"
		}

	