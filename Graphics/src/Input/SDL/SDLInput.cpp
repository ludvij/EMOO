#include "SDLInput.hpp"
#include <format>
#include <lud_assert.hpp>
#include <print>
#include <SDL.h>

namespace Ui
{
static SDL_GameControllerButton get_sdl_equivalent(Button b)
{
	switch (b)
	{
	case Ui::Button::FACE_DOWN:     return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A;
	case Ui::Button::FACE_LEFT:     return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B;
	case Ui::Button::FACE_UP:     return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y;
	case Ui::Button::FACE_RIGHT:     return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X;
	case Ui::Button::DPAD_UP:    return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP;
	case Ui::Button::DPAD_DOWN:  return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case Ui::Button::DPAD_LEFT:  return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	case Ui::Button::DPAD_RIGHT: return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	case Ui::Button::START:      return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START;
	case Ui::Button::SELECT:	 return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK;
	case Ui::Button::R1:		 return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	case Ui::Button::R3:		 return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	case Ui::Button::L1:		 return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	case Ui::Button::L3:		 return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK;
	case Ui::Button::MISC:       return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MISC1;
	default:                     return SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_INVALID;
	}
}
SDLInput::SDLInput()
{
	Lud::check::eq(SDL_Init(SDL_INIT_GAMECONTROLLER), 0, std::format("Could not start input system, error: [{:s}]", SDL_GetError()));
	// we will select first one by default
	int n_joysticks = SDL_NumJoysticks();

	for (int i = 0; i < n_joysticks; i++)
	{
		if (SDL_IsGameController(i))
		{
			std::println("Detected controller index: [{:d}], name: [{:s}]", i, SDL_GameControllerNameForIndex(i));
			m_controller = SDL_GameControllerOpen(i);
		}
	}
}
SDLInput::~SDLInput()
{
	SDL_GameControllerClose(m_controller);
	SDL_Quit();
}
bool SDLInput::GetButton(Button b)
{
	const auto p = SDL_GameControllerGetButton(m_controller, get_sdl_equivalent(b));
	return p;
}

}
