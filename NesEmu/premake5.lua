project "NesEmu"
	kind "StaticLib"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp", 
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	includedirs {
		"src"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Test"
		defines {
			"NES_EMU_TEST"
		}
		runtime "debug"
		symbols "On"

		links {
			"vendor/googletest/googletest"
		}

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

		
