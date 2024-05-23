#ifndef GRAPHICS_SDL_INPUT_HEADER
#define GRAPHICS_SDL_INPUT_HEADER
#include "Input/Input.hpp"
#include <SDL3/SDL.h>

namespace Ui
{
class SDLInput : public IInput
{
public:
	SDLInput();
	virtual ~SDLInput() override;
	// Inherited via IInput
	bool GetButton(Button b) override;

private:
	void add_controller();
	void remove_controller();

private:
	SDL_Gamepad* m_controller{ nullptr };

	// Inherited via IInput
	void ProcessEvents(void* event) override;
};
}
#endif GRAPHICS_SDL_INPUT_HEADER