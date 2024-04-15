VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["glm"] = "%{VULKAN_SDK}/Include"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["imgui"] = "../vendor/ImGui"
IncludeDir["sdl2"] = "vendor/sdl2/include"
IncludeDir["NesEmu"] = "../NesEmu/src"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["tiny_gltf"] = "vendor/tiny_gltf"
IncludeDir["json"] = "vendor/json"
IncludeDir["lud_utils"] = "../vendor/lud_utils"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["sdl2"] = "vendor/sdl2/lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["sdl2"] = "%{LibraryDir.sdl2}/SDL2"



