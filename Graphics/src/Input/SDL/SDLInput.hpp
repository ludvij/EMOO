#ifndef GRAPHICS_SDL_INPUT_HEADER
#define GRAPHICS_SDL_INPUT_HEADER
#include "Input/Input.hpp"
#include <SDL.h>

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
	SDL_GameController* m_controller{ nullptr };
};
}
#endif GRAPHICS_SDL_INPUT_HEADER