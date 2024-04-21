VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"]     = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["glm"]           = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["imgui"]         = "%{wks.location}/vendor/ImGui"
IncludeDir["lud_utils"]     = "%{wks.location}/vendor/lud_utils"
IncludeDir["fastgltf"]      = "%{wks.location}/vendor/fastgltf/include"
IncludeDir["sdl2"]          = "%{wks.location}/Graphics/vendor/sdl2/include"
IncludeDir["stb_image"]     = "%{wks.location}/Graphics/vendor/stb_image"
IncludeDir["vk_bootrstrap"] = "%{wks.location}/Graphics/vendor/vk_bootstrap"
IncludeDir["NesEmu"]        = "%{wks.location}/NesEmu/src"
IncludeDir["ctre"]          = "%{wks.location}/NesEmu/vendor/ctre/include"
IncludeDir["gtest"]         = "%{wks.location}/NesEmu/vendor/gtest/include"
IncludeDir["freetype"]      = "%{wks.location}/vendor/freetype/include"

LibraryDir = {}
LibraryDir["VulkanSDK"]     = "%{wks.location}/%{VULKAN_SDK}/Lib"
LibraryDir["sdl2"]          = "%{wks.location}/Graphics/vendor/sdl2/lib"
LibraryDir["gtest_debug"]   = "%{wks.location}/NesEmu/vendor/gtest/lib/debug"
LibraryDir["gtest_release"] = "%{wks.location}/NesEmu/vendor/gtest/lib/release"
LibraryDir["freetype_r"]    = "%{wks.location}/vendor/freetype/lib/release"
LibraryDir["freetype_d"]    = "%{wks.location}/vendor/freetype/lib/debug"

Library = {}
Library["Vulkan"]        = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["sdl2"]          = "%{LibraryDir.sdl2}/SDL2"
Library["gtest_debug"]   = "%{LibraryDir.gtest_debug}/googletest"
Library["gtest_release"] = "%{LibraryDir.gtest_release}/googletest"
Library["freetype_r"]    = "%{LibraryDir.freetype_r}/freetype"
Library["freetype_d"]    = "%{LibraryDir.freetype_d}/freetype"



