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
		-- "test/**.hpp",

		"%{IncludeDir.ctre}/**.hpp",
	}

	flags {
		"FatalWarnings"
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	includedirs {
		"src",
		"%{IncludeDir.ctre}",
		"%{IncludeDir.gtest}",
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
			-- "%{Library.gtest_debug}"
		}

	filter "configurations:Release"
		defines { 
			"NES_EMU_NDEBUG" 
		}
		links {
			-- "%{Library.gtest_release}"
		}
		runtime "release"
		optimize "On"