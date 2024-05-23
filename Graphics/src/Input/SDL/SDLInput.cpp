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

	add_controller();
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

void SDLInput::add_controller()
{
	if (m_controller != nullptr)
	{
		return;
	}
	int count;
	SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&count);
	for (size_t i = 0; i < count; i++)
	{
		const SDL_JoystickID curr = joystick_ids[i];
		if (SDL_IsGamepad(curr))
		{
			m_controller = SDL_OpenGamepad(curr);
			std::println("Added controller ID: [{:d}], name: [{:s}]", curr, SDL_GetGamepadName(m_controller));
		}
	}
	SDL_free(joystick_ids);
}

void SDLInput::remove_controller()
{
	if (m_controller == nullptr)
	{
		return;
	}
	int count;
	SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&count);
	auto id = SDL_GetGamepadInstanceID(m_controller);
	if (std::find(joystick_ids, joystick_ids + count, id) == joystick_ids + count)
	{
		std::println("Removed controller ID: [{:d}], name: [{:s}]", id, SDL_GetGamepadName(m_controller));
		SDL_CloseGamepad(m_controller);
		m_controller = nullptr;
	}
	SDL_free(joystick_ids);
}

void SDLInput::ProcessEvents(void* event)
{
	auto sdl_e = static_cast<SDL_Event*>( event );
	switch (sdl_e->type)
	{
	case SDL_EVENT_GAMEPAD_ADDED:
		add_controller();
		break;
	case SDL_EVENT_GAMEPAD_REMOVED:
		remove_controller();
		break;
	default:
		break;
	}
}

}
