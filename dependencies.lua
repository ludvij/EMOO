VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"]     = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["glm"]           = "%{wks.location}/%{VULKAN_SDK}/Include"
IncludeDir["imgui"]         = "%{wks.location}/vendor/ImGui"
IncludeDir["lud_utils"]     = "%{wks.location}/vendor/lud_utils/include"
IncludeDir["vk_bootrstrap"] = "%{wks.location}/Renderer/vendor/vk_bootstrap"
IncludeDir["pfd"]           = "%{wks.location}/Application/vendor/pfd/include"
IncludeDir["ctre"]          = "%{wks.location}/NesEmu/vendor/ctre/include"
IncludeDir["gtest"]         = "%{wks.location}/vendor/googletest/googletest/include"
IncludeDir["NesEmu"]        = "%{wks.location}/NesEmu/src"
IncludeDir["Renderer"]      = "%{wks.location}/Renderer/src"
IncludeDir["Input"]         = "%{wks.location}/Input/include"
IncludeDir["Window"]        = "%{wks.location}/Window/include"
IncludeDir["FileManager"]   = "%{wks.location}/FileManager/include"
IncludeDir["cppicons"]      = "%{wks.location}/Application/vendor/cppicons/include"
IncludeDir["sdl"]           = "%{wks.location}/vendor/sdl2/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{wks.location}/%{VULKAN_SDK}/Lib"
LibraryDir["sdl_d"]     = "%{wks.location}/vendor/sdl2/lib/debug"
LibraryDir["sdl_r"]     = "%{wks.location}/vendor/sdl2/lib/release"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["sdl_d"]  = "%{LibraryDir.sdl_d}/SDL2-staticd"
Library["sdl_r"]  = "%{LibraryDir.sdl_r}/SDL2-static"


