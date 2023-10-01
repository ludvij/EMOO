project "TFG"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp", 
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
