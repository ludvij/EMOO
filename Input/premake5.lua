project "Input"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	platform = 'SDL3' and sdl3 or 'SDL2'

	files { 
		"include/Input/*.hpp", 
		"src/*.cpp",
		"include/Input/" .. platform .. "/**.hpp",
		"src/" .. platform .. "/**.cpp"
	}

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"include/Input",
		"%{IncludeDir.sdl}",
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

