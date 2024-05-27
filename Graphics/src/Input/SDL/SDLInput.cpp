#include "SDLInput.hpp"
#include <format>
#include <lud_assert.hpp>
#include <print>
#include <SDL3/SDL.h>

namespace Input
{
static SDL_GamepadButton get_sdl_equivalent(Button b)
{
	switch (b)
	{
	case Input::Button::FACE_DOWN:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH;
	case Input::Button::FACE_LEFT:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_EAST;
	case Input::Button::FACE_UP:    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH;
	case Input::Button::FACE_RIGHT: return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST;
	case Input::Button::DPAD_UP:    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP;
	case Input::Button::DPAD_DOWN:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN;
	case Input::Button::DPAD_LEFT:  return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT;
	case Input::Button::DPAD_RIGHT: return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
	case Input::Button::START:      return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START;
	case Input::Button::SELECT:	    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_BACK;
	case Input::Button::R1:		    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
	case Input::Button::R3:		    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_STICK;
	case Input::Button::L1:		    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
	case Input::Button::L3:		    return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_STICK;
	case Input::Button::MISC:       return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_MISC1;
	default:                        return SDL_GamepadButton::SDL_GAMEPAD_BUTTON_INVALID;
	}
}

static SDL_Scancode get_sdl_equivalent(Key k)
{
	switch (k)
	{
	case Input::Key::NONE: return SDL_Scancode::SDL_SCANCODE_UNKNOWN;
	case Input::Key::F1: return SDL_Scancode::SDL_SCANCODE_F1;
	case Input::Key::F2: return SDL_Scancode::SDL_SCANCODE_F2;
	case Input::Key::F3: return SDL_Scancode::SDL_SCANCODE_F3;
	case Input::Key::F4: return SDL_Scancode::SDL_SCANCODE_F4;
	case Input::Key::F5: return SDL_Scancode::SDL_SCANCODE_F5;
	case Input::Key::F6: return SDL_Scancode::SDL_SCANCODE_F6;
	case Input::Key::F7: return SDL_Scancode::SDL_SCANCODE_F7;
	case Input::Key::F8: return SDL_Scancode::SDL_SCANCODE_F8;
	case Input::Key::F9: return SDL_Scancode::SDL_SCANCODE_F9;
	case Input::Key::F10: return SDL_Scancode::SDL_SCANCODE_F10;
	case Input::Key::F11: return SDL_Scancode::SDL_SCANCODE_F11;
	case Input::Key::F12: return SDL_Scancode::SDL_SCANCODE_F12;
	case Input::Key::F13: return SDL_Scancode::SDL_SCANCODE_F13;
	case Input::Key::F14: return SDL_Scancode::SDL_SCANCODE_F14;
	case Input::Key::F15: return SDL_Scancode::SDL_SCANCODE_F15;
	case Input::Key::F16: return SDL_Scancode::SDL_SCANCODE_F16;
	case Input::Key::F17: return SDL_Scancode::SDL_SCANCODE_F17;
	case Input::Key::F18: return SDL_Scancode::SDL_SCANCODE_F18;
	case Input::Key::F19: return SDL_Scancode::SDL_SCANCODE_F19;
	case Input::Key::F20: return SDL_Scancode::SDL_SCANCODE_F20;
	case Input::Key::F21: return SDL_Scancode::SDL_SCANCODE_F21;
	case Input::Key::F22: return SDL_Scancode::SDL_SCANCODE_F22;
	case Input::Key::F23: return SDL_Scancode::SDL_SCANCODE_F23;
	case Input::Key::F24: return SDL_Scancode::SDL_SCANCODE_F24;
	case Input::Key::K0: return SDL_Scancode::SDL_SCANCODE_0;
	case Input::Key::K1: return SDL_Scancode::SDL_SCANCODE_1;
	case Input::Key::K2: return SDL_Scancode::SDL_SCANCODE_2;
	case Input::Key::K3: return SDL_Scancode::SDL_SCANCODE_3;
	case Input::Key::K4: return SDL_Scancode::SDL_SCANCODE_4;
	case Input::Key::K5: return SDL_Scancode::SDL_SCANCODE_5;
	case Input::Key::K6: return SDL_Scancode::SDL_SCANCODE_6;
	case Input::Key::K7: return SDL_Scancode::SDL_SCANCODE_7;
	case Input::Key::K8: return SDL_Scancode::SDL_SCANCODE_8;
	case Input::Key::K9: return SDL_Scancode::SDL_SCANCODE_9;
	case Input::Key::A: return SDL_Scancode::SDL_SCANCODE_A;
	case Input::Key::B: return SDL_Scancode::SDL_SCANCODE_B;
	case Input::Key::C: return SDL_Scancode::SDL_SCANCODE_C;
	case Input::Key::D: return SDL_Scancode::SDL_SCANCODE_D;
	case Input::Key::E: return SDL_Scancode::SDL_SCANCODE_E;
	case Input::Key::F: return SDL_Scancode::SDL_SCANCODE_F;
	case Input::Key::G: return SDL_Scancode::SDL_SCANCODE_G;
	case Input::Key::H: return SDL_Scancode::SDL_SCANCODE_H;
	case Input::Key::I: return SDL_Scancode::SDL_SCANCODE_I;
	case Input::Key::J: return SDL_Scancode::SDL_SCANCODE_J;
	case Input::Key::K: return SDL_Scancode::SDL_SCANCODE_K;
	case Input::Key::L: return SDL_Scancode::SDL_SCANCODE_L;
	case Input::Key::M: return SDL_Scancode::SDL_SCANCODE_M;
	case Input::Key::N: return SDL_Scancode::SDL_SCANCODE_N;
	case Input::Key::O: return SDL_Scancode::SDL_SCANCODE_O;
	case Input::Key::P: return SDL_Scancode::SDL_SCANCODE_P;
	case Input::Key::Q: return SDL_Scancode::SDL_SCANCODE_Q;
	case Input::Key::R: return SDL_Scancode::SDL_SCANCODE_R;
	case Input::Key::S: return SDL_Scancode::SDL_SCANCODE_S;
	case Input::Key::T: return SDL_Scancode::SDL_SCANCODE_T;
	case Input::Key::U: return SDL_Scancode::SDL_SCANCODE_U;
	case Input::Key::V: return SDL_Scancode::SDL_SCANCODE_V;
	case Input::Key::W: return SDL_Scancode::SDL_SCANCODE_W;
	case Input::Key::X: return SDL_Scancode::SDL_SCANCODE_X;
	case Input::Key::Y: return SDL_Scancode::SDL_SCANCODE_Y;
	case Input::Key::Z: return SDL_Scancode::SDL_SCANCODE_Z;
	case Input::Key::CAPSLOCK: return SDL_Scancode::SDL_SCANCODE_CAPSLOCK;
	case Input::Key::SPACE: return SDL_Scancode::SDL_SCANCODE_SPACE;
	case Input::Key::TAB: return SDL_Scancode::SDL_SCANCODE_TAB;
	case Input::Key::ENTER: return SDL_Scancode::SDL_SCANCODE_RETURN;
	case Input::Key::ESCAPE: return SDL_Scancode::SDL_SCANCODE_ESCAPE;
	case Input::Key::BACK: return SDL_Scancode::SDL_SCANCODE_BACKSPACE;
	case Input::Key::LWIN: return SDL_Scancode::SDL_SCANCODE_LGUI;
	case Input::Key::RWIN: return SDL_Scancode::SDL_SCANCODE_RGUI;
	case Input::Key::LCTRL: return SDL_Scancode::SDL_SCANCODE_LCTRL;
	case Input::Key::RCTRL: return SDL_Scancode::SDL_SCANCODE_RCTRL;
	case Input::Key::LALT: return SDL_Scancode::SDL_SCANCODE_LALT;
	case Input::Key::RALT: return SDL_Scancode::SDL_SCANCODE_RALT;
	case Input::Key::LSHIFT: return SDL_Scancode::SDL_SCANCODE_LSHIFT;
	case Input::Key::RSHIFT: return SDL_Scancode::SDL_SCANCODE_RSHIFT;
	case Input::Key::SCROLLLOCK: return SDL_Scancode::SDL_SCANCODE_SCROLLLOCK;
	case Input::Key::DELETE: return SDL_Scancode::SDL_SCANCODE_DELETE;
	case Input::Key::INSERT: return SDL_Scancode::SDL_SCANCODE_INSERT;
	case Input::Key::HOME: return SDL_Scancode::SDL_SCANCODE_HOME;
	case Input::Key::END: return SDL_Scancode::SDL_SCANCODE_END;
	case Input::Key::PAGEUP: return SDL_Scancode::SDL_SCANCODE_PAGEUP;
	case Input::Key::PAGEDOWN: return SDL_Scancode::SDL_SCANCODE_PAGEDOWN;
	case Input::Key::UP: return SDL_Scancode::SDL_SCANCODE_UP;
	case Input::Key::DOWN: return SDL_Scancode::SDL_SCANCODE_DOWN;
	case Input::Key::LEFT: return SDL_Scancode::SDL_SCANCODE_LEFT;
	case Input::Key::RIGHT: return SDL_Scancode::SDL_SCANCODE_RIGHT;
	case Input::Key::NUMLOCK: return SDL_Scancode::SDL_SCANCODE_NUMLOCKCLEAR;
	case Input::Key::NP0: return SDL_Scancode::SDL_SCANCODE_KP_0;
	case Input::Key::NP1: return SDL_Scancode::SDL_SCANCODE_KP_1;
	case Input::Key::NP2: return SDL_Scancode::SDL_SCANCODE_KP_2;
	case Input::Key::NP3: return SDL_Scancode::SDL_SCANCODE_KP_3;
	case Input::Key::NP4: return SDL_Scancode::SDL_SCANCODE_KP_4;
	case Input::Key::NP5: return SDL_Scancode::SDL_SCANCODE_KP_5;
	case Input::Key::NP6: return SDL_Scancode::SDL_SCANCODE_KP_6;
	case Input::Key::NP7: return SDL_Scancode::SDL_SCANCODE_KP_7;
	case Input::Key::NP8: return SDL_Scancode::SDL_SCANCODE_KP_8;
	case Input::Key::NP9: return SDL_Scancode::SDL_SCANCODE_KP_9;
	case Input::Key::NPADD: return SDL_Scancode::SDL_SCANCODE_KP_PLUS;
	case Input::Key::NPSUB: return SDL_Scancode::SDL_SCANCODE_KP_MINUS;
	case Input::Key::NPDIV: return SDL_Scancode::SDL_SCANCODE_KP_DIVIDE;
	case Input::Key::NPMUL: return SDL_Scancode::SDL_SCANCODE_KP_MULTIPLY;
	case Input::Key::NPENTER: return SDL_Scancode::SDL_SCANCODE_KP_ENTER;
	case Input::Key::NPPERIOD: return SDL_Scancode::SDL_SCANCODE_KP_PERIOD;
	case Input::Key::GRAVE: return SDL_Scancode::SDL_SCANCODE_GRAVE;
	default: return SDL_SCANCODE_UNKNOWN;
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
bool SDLInput::GetButton(Button b) const
{
	const auto p = SDL_GetGamepadButton(m_controller, get_sdl_equivalent(b));
	return p;
}

bool SDLInput::GetKey(Key k) const
{
	return m_keyboard_state[get_sdl_equivalent(k)] == 1;
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

void SDLInput::platform_override()
{
	m_keyboard_state = SDL_GetKeyboardState(NULL);
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
