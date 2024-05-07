VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"]     = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["glm"]           = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["imgui"]         = "%{wks.location}/vendor/ImGui"
IncludeDir["lud_utils"]     = "%{wks.location}/vendor/lud_utils"
IncludeDir["sdl2"]          = "%{wks.location}/Graphics/vendor/sdl2/include"
IncludeDir["vk_bootrstrap"] = "%{wks.location}/Graphics/vendor/vk_bootstrap"
IncludeDir["pfd"]           = "%{wks.location}/Graphics/vendor/pfd/include"
IncludeDir["NesEmu"]        = "%{wks.location}/NesEmu/src"
IncludeDir["ctre"]          = "%{wks.location}/NesEmu/vendor/ctre/include"
IncludeDir["gtest"]         = "%{wks.location}/NesEmu/vendor/gtest/include"
IncludeDir["freetype"]      = "%{wks.location}/vendor/freetype/include"

LibraryDir = {}
LibraryDir["VulkanSDK"]  = "%{wks.location}/%{VULKAN_SDK}/Lib"
LibraryDir["sdl2"]       = "%{wks.location}/Graphics/vendor/sdl2/lib"
LibraryDir["gtest_d"]    = "%{wks.location}/NesEmu/vendor/gtest/lib/debug"
LibraryDir["gtest_r"]    = "%{wks.location}/NesEmu/vendor/gtest/lib/release"
LibraryDir["freetype_r"] = "%{wks.location}/vendor/freetype/lib/release"
LibraryDir["freetype_d"] = "%{wks.location}/vendor/freetype/lib/debug"

Library = {}
Library["Vulkan"]     = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["sdl2"]       = "%{LibraryDir.sdl2}/SDL2"
Library["gtest_d"]    = "%{LibraryDir.gtest_d}/gtest"
Library["gtest_r"]    = "%{LibraryDir.gtest_r}/gtest"
Library["freetype_r"] = "%{LibraryDir.freetype_r}/freetype"
Library["freetype_d"] = "%{LibraryDir.freetype_d}/freetype"