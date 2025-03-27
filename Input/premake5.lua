project "Input"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")


	files { 
		"include/Input/*.hpp", 
		"src/*.cpp",
		"include/Input/SDL2/**.hpp",
		"src/SDL2/**.cpp"
	}

	fatalwarnings { "All" }

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

