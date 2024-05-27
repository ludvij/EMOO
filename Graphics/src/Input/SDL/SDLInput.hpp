#ifndef GRAPHICS_SDL_INPUT_HEADER
#define GRAPHICS_SDL_INPUT_HEADER
#include "Input/Input.hpp"
#include <SDL3/SDL.h>

namespace Input
{
class SDLInput : public IInput
{
public:
	SDLInput();
	virtual ~SDLInput() override;
	// Inherited via IInput
	bool GetButton(Button b) const override;
	bool GetKey(Key k) const override;

private:
	void add_controller();
	void remove_controller();

protected:
	virtual void platform_override() override;

private:
	SDL_Gamepad* m_controller{ nullptr };

	const Uint8* m_keyboard_state{ nullptr };

	// Inherited via IInput
	void ProcessEvents(void* event) override;
};
}
#endif GRAPHICS_SDL_INPUT_HEADER