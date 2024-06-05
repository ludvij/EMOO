project "Input"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"include/Input/*.hpp", 
		"src/*.cpp",
	}
	if (sdl3) then
		files {
			"include/Input/SDL3/**.hpp",
			"src/SDL3/**.cpp"
		}
	else
		files {
			"include/Input/SDL2/**.hpp",
			"src/SDL2/**.cpp"
		}
	end

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"include/Input",
		sdl3 and "%{IncludeDir.sdl3}" or "%{IncludeDir.sdl2}",
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

