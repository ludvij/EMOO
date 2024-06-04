project "Renderer"
	kind "StaticLib"
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

		"Fonts/**.embed",

		"Shader/**.comp",
		"Shader/**.frag",
		"Shader/**.vert",
		"Shader/**.glsl"
	}

	flags {
	}


	-- pchheader "pch.hpp"
	-- pchsource "src/pch.cpp"

	includedirs {
		"src",
		".",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.vk_bootrstrap}",
		"%{IncludeDir.Window}",
	}


	links {
		"ImGui",
		"Input",
		"Window",
		"%{Library.Vulkan}",
	}

	defines {
		"GLM_ENABLE_EXPERIMENTAL",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
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
		}
	
	filter "configurations:Release"
		kind "WindowedApp"
		defines { 
			"GRAPHICS_NDEBUG",
			"NDEBUG"
		}
		links {

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



	