#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Ui
{
class SDL
{
using NativeType = SDL_Window*;
public:
	static SDL& Get();
	~SDL();

	void Init(const char* name, uint32_t w, uint32_t h);

	std::vector<const char*> GetInstanceExtensions();


	SDL(SDL const&)            = delete;
	void operator=(SDL const&) = delete;
	operator NativeType() { return m_window; };

private:
	SDL() = default;

private:
	NativeType m_window = nullptr;
};
}

