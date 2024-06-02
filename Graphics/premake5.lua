project "Graphics"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	targetname ("emoo")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.h", 
		"src/**.cpp",

		"%{IncludeDir.vk_bootrstrap}/**.cpp",
		"%{IncludeDir.vk_bootrstrap}/**.h",
		"%{IncludeDir.lud_utils}/**.hpp",

		"src/**.embed",

		"Shader/**.comp",
		"Shader/**.frag",
		"Shader/**.vert",
		"Shader/**.glsl",

		"assets/resource.rc"
	}

	flags {
	}


	-- pchheader "pch.hpp"
	-- pchsource "src/pch.cpp"

	includedirs {
		"src",
		"%{IncludeDir.sdl3}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.vk_bootrstrap}",
		"%{IncludeDir.nativeFileDialog}",
		"%{IncludeDir.pfd}",

		"%{IncludeDir.Input}",
		"%{IncludeDir.NesEmu}",
	}


	links {
		"ImGui",
		"NesEmu",
		"Input",
		"%{Library.Vulkan}",
		-- sdl complains
		"winmm",
		"setupapi",
		"version",
		"Imm32"
	}

	defines {
		"SDL_MAIN_HANDLED",
		"GLM_ENABLE_EXPERIMENTAL",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"SPECTRUM_USE_DARK_THEME"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS",
			"WIN32",
			"GRAPHICS_SHOW_FPS"
		}
		
	

	filter "configurations:Debug"
		defines { 
			"GRAPHICS_DEBUG",
			"TRACE_UUID_LIFETIMES",
		}
		runtime "debug"
		symbols "On"

		links {
			"%{Library.freetype_d}",
			"%{Library.sdl3_d}",
		}
	
	filter "configurations:Release"
		kind "WindowedApp"
		defines { 
			"GRAPHICS_NDEBUG",
			"NDEBUG"
		}
		links {
			"%{Library.freetype_r}",
			"%{Library.sdl3_r}",

		}

		runtime "release"
		optimize "On"


	filter 'files:Shader/*.vert or Shader/*.frag or Shader/*.comp' 
		buildcommands {
			'%{VULKAN_SDK}/BIN/glslangValidator -V -o "%{file.directory}/SPIRV/%{file.name}.spv" "%{file.relpath}"',
			'"%{wks.location}/utility/spv_to_embed.py" "%{file.directory}/SPIRV/%{file.name}.spv" > "%{file.directory}/embed/%{file.name}.cpp"',
		}

		buildoutputs {
			"%{file.directory}/embed/%{file.name}.cpp",
		}



	