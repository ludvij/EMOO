#ifndef GRAPHICS_SDL_INPUT_HEADER
#define GRAPHICS_SDL_INPUT_HEADER
#include "../Input.hpp"
#include <SDL.h>

namespace Input
{
class SDL2Input : public IInput
{
public:
	SDL2Input();
	virtual ~SDL2Input() override;
	// Inherited via IInput
	bool GetButton(int trigger) const override;
	bool GetKey(int trigger) const override;

private:
	void add_controller();
	void remove_controller(Sint32 id);

protected:
	virtual void platform_override() override;

private:
	SDL_GameController* m_controller{ nullptr };

	const Uint8* m_keyboard_state{ nullptr };

	// Inherited via IInput
	void ProcessEvents(void* event) override;
};
}
#endif //GRAPHICS_SDL_INPUT_HEADER