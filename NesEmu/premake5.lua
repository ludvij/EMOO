project "NesEmu"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp",
		"test/**.cpp"
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	includedirs {
		"src",
		"vendor/include"
	}

	filter "system:windows"
		systemversion "latest"
	
	
-- Uncomment to disable tests
	links {
		"googletest",
		"vendor/lib/lua546/lua5.4.6.lib"
	}

	includedirs {
		"vendor/googletest/googletest/include"
	}

	defines {
		"NES_EMU_TEST"
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