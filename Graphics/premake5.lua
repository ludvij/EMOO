project "Graphics"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.h", 
		"src/**.cpp",

		"%{IncludeDir.vk_bootrstrap}/**.cpp",
		"%{IncludeDir.vk_bootrstrap}/**.h",
		"%{IncludeDir.stb_image}/**.h",
		"%{IncludeDir.lud_utils}/**.hpp",

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
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.NesEmu}",
		"%{IncludeDir.fastgltf}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.vk_bootrstrap}",
	}


	links {
		"ImGui",
		"NesEmu",
		"fastgltf",
		"%{Library.sdl2}",
		"%{Library.Vulkan}"
	}

	defines {
		"SDL_MAIN_HANDLED",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS",
			"WIN32",
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


	filter 'files:Shader/*'
		buildcommands {
			'%{VULKAN_SDK}/BIN/glslangValidator -V -o "%{file.directory}/SPIRV/%{file.name}.spv" "%{file.relpath}"'
		}

		buildoutputs {
			"%{file.directory}/SPIRV/%{file.name}.spv"
		}

	