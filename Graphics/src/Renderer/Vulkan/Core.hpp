#ifndef GRAPHICS_RENDERER_CORE_HEADER
#define GRAPHICS_RENDERER_CORE_HEADER

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>
#include <iostream>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <vma/vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

constexpr auto VK_CHECK = [](vk::Result err)
{
    if (err != vk::Result::eSuccess)
    {
        std::cerr << "Detected Vulkan error: " << vk::to_string(err) << '\n';
        throw std::runtime_error(vk::to_string(err));
    }
};
#endif