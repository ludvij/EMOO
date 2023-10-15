-- https://github.com/babaliaris/googletest/blob/main/googlemock/premake5.lua
project "googlemock"
	staticruntime "on"
	kind "StaticLib"
	language "C++"
	cppdialect "c++17"
	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputDir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cc",
		"include/**.h"
	}

	includedirs {
		".",
		"include/",
		"../googletest/include/"

	}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { 
			"NDEBUG" 
		}
		runtime "release"
		optimize "On"


	filter "system:Windows"
		buildoptions {
			"-GS", "-W4", "-WX", "-wd4251", "-wd4275", "-nologo", "-J",
			"-D_UNICODE", "-DUNICODE", "-DWIN32", "-D_WIN32",
			"-EHs-c-", "-D_HAS_EXCEPTIONS=0", "-GR-", "-wd4702", "-utf-8"
		}
		defines {
			"GTEST_OS_WINDOWS"
		}