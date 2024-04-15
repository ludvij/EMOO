project "NesEmu"
	kind "StaticLib"
	language "C++"
	cppdialect "c++20"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp",
		-- "test/**.cpp",
		-- "test/**.hpp"
	}

	flags {
		"FatalWarnings"
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	includedirs {
		"src",
		"vendor/include"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"NES_EMU_PLATFORM_WINDOWS"
		}
	
	postbuildcommands {
		-- "{COPYDIR} %[palettes] %[../Graphics/Palettes]"
	}
	
	
-- Uncomment to disable tests


	-- defines {
	-- 	"NES_EMU_TEST"
	-- }
	

	filter "configurations:Debug"
		defines { 
			"NES_EMU_DEBUG" 
		}
		runtime "debug"
		symbols "On"
		links {
			-- "vendor/lib/debug/googletest.lib"
		}

	filter "configurations:Release"
		defines { 
			"NES_EMU_NDEBUG" 
		}
		links {
			-- "vendor/lib/release/googletest.lib"
		}
		runtime "release"
		optimize "On"