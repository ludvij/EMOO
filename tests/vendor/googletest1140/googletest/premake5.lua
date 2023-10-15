-- https://github.com/babaliaris/googletest/blob/main/googletest/premake5.lua

project "googletest"
	staticruntime "on"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cc",
		"include/**.h"
	}

	includedirs {
		".",
		"include/"
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

	filter "system:windows"
		buildoptions {
			"-GS", "-W4", "-WX", "-wd4251", "-wd4275", "-nologo", "-J",
			"-D_UNICODE", "-DUNICODE", "-DWIN32", "-D_WIN32",
			"-EHs-c-", "-D_HAS_EXCEPTIONS=0", "-GR-", "-wd4702", "-utf-8"
		}
		systemversion "latest"
		defines {
			"GTEST_OS_WINDOWS"
		}
