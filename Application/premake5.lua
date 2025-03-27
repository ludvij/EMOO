project "Application"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "on"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	targetname ("emoo")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.h", 
		"src/**.cpp",

		"%{IncludeDir.lud_utils}/**.hpp",
		"%{IncludeDir.cppicons}/**.hpp",

		"src/**.embed",

		"resources/resource.rc",
		"resources/texts/*.txf",
		"resources/fonts/*.embed"
	}

	flags {
	}


	-- pchheader "pch.hpp"
	-- pchsource "src/pch.cpp"

	includedirs {
		"src",
		"%{IncludeDir.sdl}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.vk_bootrstrap}",
		"%{IncludeDir.nativeFileDialog}",
		"%{IncludeDir.pfd}",
		"%{IncludeDir.cppicons}",

		"%{IncludeDir.Input}",
		"%{IncludeDir.Window}",
		"%{IncludeDir.NesEmu}",
		"%{IncludeDir.Renderer}",
		"%{IncludeDir.FileManager}",
	}


	links {
		"ImGui",
		"NesEmu",
		"Input",
		"Window",
		"FileManager",
		"Renderer",
		"%{Library.Vulkan}",
		-- sdl complains
		"winmm",
		"setupapi",
		"version",
		"Imm32"
	}

	defines {
		"SDL_MAIN_HANDLED",
		"IMGUI_DEFINE_MATH_OPERATORS"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS",
			"WIN32",
		}
		
	

	filter "configurations:Debug"
		defines { 
			"GRAPHICS_DEBUG",
		}
		runtime "debug"
		symbols "On"

		links {
			"%{Library.sdl_d}",
		}
	
	filter "configurations:Release"
		kind "WindowedApp"
		defines { 
			"GRAPHICS_NDEBUG",
			"NDEBUG"
		}
		links {
			"%{Library.sdl_r}",
		}

		runtime "release"
		optimize "On"

	filter 'files:resources/texts/*.txf' 
		buildcommands {
			'"%{prj.location}/make_ex_file.py"'
		}

		buildoutputs {
			"%{prj.location}/src/embedding/explanations.hpp",
		}


	