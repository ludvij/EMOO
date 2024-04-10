#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Ui
{
class Window
{
using NativeType = SDL_Window*;
public:
	static Window& Get();
	~Window();

	void Init(const char* name, uint32_t w, uint32_t h);

	Window(Window const&)            = delete;
	void operator=(Window const&) = delete;
	operator NativeType() { return m_window; };

private:
	Window() = default;

private:
	NativeType m_window = nullptr;
};
}

