#ifndef GRAPHICS_SDL_INPUT_HEADER
#define GRAPHICS_SDL_INPUT_HEADER
#include "Input.hpp"
#include <SDL.h>

namespace Input
{
class SDL3Input : public IInput
{
public:
	SDL3Input();
	virtual ~SDL3Input() override;
	// Inherited via IInput
	bool GetButton(Button b) const override;
	bool GetKey(Key k) const override;

private:
	void add_controller();
	void remove_controller();

protected:
	virtual void platform_override() override;

private:
	SDL_GameController* m_controller{ nullptr };

	const Uint8* m_keyboard_state{ nullptr };

	// Inherited via IInput
	void ProcessEvents(void* event) override;
};
}
#endif GRAPHICS_SDL_INPUT_HEADER