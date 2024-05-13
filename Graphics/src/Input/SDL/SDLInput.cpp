#include "SDLInput.hpp"
#include <format>
#include <lud_assert.hpp>
#include <print>
#include <SDL3/SDL.h>

namespace Ui
{
static SDL_GamepadButton get_sdl_equivalent(Button b)
{
	switch (b)
	{
	case Ui::Button::FACE_DOWN:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH;
	case Ui::Button::FACE_LEFT:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_EAST;
	case Ui::Button::FACE_UP:    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH;
	case Ui::Button::FACE_RIGHT: return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST;
	case Ui::Button::DPAD_UP:    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP;
	case Ui::Button::DPAD_DOWN:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN;
	case Ui::Button::DPAD_LEFT:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT;
	case Ui::Button::DPAD_RIGHT: return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
	case Ui::Button::START:      return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START;
	case Ui::Button::SELECT:	 return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_BACK;
	case Ui::Button::R1:		 return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
	case Ui::Button::R3:		 return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_STICK;
	case Ui::Button::L1:		 return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
	case Ui::Button::L3:		 return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_STICK;
	case Ui::Button::MISC:       return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_MISC1;
	default:                     return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_INVALID;
	}
}
SDLInput::SDLInput()
{
	Lud::check::eq(SDL_Init(SDL_INIT_GAMEPAD), 0, std::format("Could not start input system, error: [{:s}]", SDL_GetError()));
	// we will select first one by default

	int count;
	SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&count);
	for (size_t i = 0; i < count; i++)
	{
		const SDL_JoystickID curr = joystick_ids[i];
		if (SDL_IsGamepad(curr))
		{
			std::println("Detected controller index: [{:d}], name: [{:s}]", i, SDL_GetGamepadInstanceName(curr));
			m_controller = SDL_OpenGamepad(curr);
		}
	}
	SDL_free(joystick_ids);
}
SDLInput::~SDLInput()
{
	SDL_CloseGamepad(m_controller);
	SDL_Quit();
}
bool SDLInput::GetButton(Button b)
{
	const auto p = SDL_GetGamepadButton(m_controller, get_sdl_equivalent(b));
	return p;
}

}
