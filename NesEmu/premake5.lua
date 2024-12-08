project "NesEmu"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files { 
		"src/**.hpp", 
		"src/**.cpp",

		"%{IncludeDir.ctre}/**.hpp",
	}

	links {
		"FileManager"
	}

	flags {
		"FatalWarnings"
	}

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	includedirs {
		"src",
		"%{IncludeDir.ctre}",
		"%{IncludeDir.lud_utils}",
		"%{IncludeDir.FileManager}",

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
			"NES_EMU_DEBUG" 
		}
		runtime "debug"
		symbols "On"

	filter "configurations:Release"
		defines { 
			"NES_EMU_NDEBUG" 
		}
		runtime "release"
		symbols "Off"
		optimize "On"

