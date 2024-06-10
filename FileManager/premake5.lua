project "FileManager"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")


	files { 
		"include/**.hpp", 
		"src/**.cpp",
	}

	flags {
		"FatalWarnings"
	}

	includedirs {
		"src",
		"include/FileManager",
		"%{IncludeDir.lud_utils}",

	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"FILE_MANAGER_PLATFORM_WINDOWS"
		}
	

	filter "configurations:Debug"
		defines { 
			"FILE_MANAGER_DEBUG" 
		}
		runtime "debug"
		symbols "On"

	filter "configurations:Release"
		defines { 
			"FILE_MANAGER_NDEBUG" 
		}
		runtime "release"
		symbols "Off"
		optimize "On"

