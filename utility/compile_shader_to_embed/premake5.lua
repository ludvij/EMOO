
project "compile_shader_to_embed"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")	

	files { 
		"src/**.cpp",
		"src/**.hpp",

		"%{IncludeDir.lud_utils}/lud_slurper.hpp",
	}
	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"%{IncludeDir.lud_utils}"
	}

	filter "system:windows"
		systemversion "latest"
	

	filter "configurations:Debug"
		defines { 
			"DEBUG" 
		}
		runtime "debug"
		symbols "On"
	
	filter "configurations:Release"
		defines { 
			"NDEBUG"
		}

		runtime "release"
		optimize "On"
		symbols "Off"