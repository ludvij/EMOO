#include "SDLInput.hpp"
#include <format>
#include <lud_assert.hpp>
#include <print>
#include <SDL3/SDL.h>

namespace Input
{
static SDL_GamepadButton get_sdl_equivalent(Button b)
{
	typedef Input::Button B;
	typedef SDL_GamepadButton S;
	switch (b)
	{
	case B::FACE_DOWN:  return S::SDL_GAMEPAD_BUTTON_SOUTH;
	case B::FACE_LEFT:  return S::SDL_GAMEPAD_BUTTON_EAST;
	case B::FACE_UP:    return S::SDL_GAMEPAD_BUTTON_NORTH;
	case B::FACE_RIGHT: return S::SDL_GAMEPAD_BUTTON_WEST;
	case B::DPAD_UP:    return S::SDL_GAMEPAD_BUTTON_DPAD_UP;
	case B::DPAD_DOWN:  return S::SDL_GAMEPAD_BUTTON_DPAD_DOWN;
	case B::DPAD_LEFT:  return S::SDL_GAMEPAD_BUTTON_DPAD_LEFT;
	case B::DPAD_RIGHT: return S::SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
	case B::START:      return S::SDL_GAMEPAD_BUTTON_START;
	case B::SELECT:	    return S::SDL_GAMEPAD_BUTTON_BACK;
	case B::R1:		    return S::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
	case B::R3:		    return S::SDL_GAMEPAD_BUTTON_RIGHT_STICK;
	case B::L1:		    return S::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
	case B::L3:		    return S::SDL_GAMEPAD_BUTTON_LEFT_STICK;
	case B::MISC:       return S::SDL_GAMEPAD_BUTTON_MISC1;
	default:            return S::SDL_GAMEPAD_BUTTON_INVALID;
	}
}

static SDL_Scancode get_sdl_equivalent(Key k)
{
	typedef Input::Key K;
	typedef SDL_Scancode S;
	switch (k)
	{
	case K::NONE:       return S::SDL_SCANCODE_UNKNOWN;
	case K::F1:         return S::SDL_SCANCODE_F1;
	case K::F2:         return S::SDL_SCANCODE_F2;
	case K::F3:         return S::SDL_SCANCODE_F3;
	case K::F4:         return S::SDL_SCANCODE_F4;
	case K::F5:         return S::SDL_SCANCODE_F5;
	case K::F6:         return S::SDL_SCANCODE_F6;
	case K::F7:         return S::SDL_SCANCODE_F7;
	case K::F8:         return S::SDL_SCANCODE_F8;
	case K::F9:         return S::SDL_SCANCODE_F9;
	case K::F10:        return S::SDL_SCANCODE_F10;
	case K::F11:        return S::SDL_SCANCODE_F11;
	case K::F12:        return S::SDL_SCANCODE_F12;
	case K::F13:        return S::SDL_SCANCODE_F13;
	case K::F14:        return S::SDL_SCANCODE_F14;
	case K::F15:        return S::SDL_SCANCODE_F15;
	case K::F16:        return S::SDL_SCANCODE_F16;
	case K::F17:        return S::SDL_SCANCODE_F17;
	case K::F18:        return S::SDL_SCANCODE_F18;
	case K::F19:        return S::SDL_SCANCODE_F19;
	case K::F20:        return S::SDL_SCANCODE_F20;
	case K::F21:        return S::SDL_SCANCODE_F21;
	case K::F22:        return S::SDL_SCANCODE_F22;
	case K::F24:        return S::SDL_SCANCODE_F24;
	case K::F23:        return S::SDL_SCANCODE_F23;
	case K::K0:         return S::SDL_SCANCODE_0;
	case K::K1:         return S::SDL_SCANCODE_1;
	case K::K2:         return S::SDL_SCANCODE_2;
	case K::K3:         return S::SDL_SCANCODE_3;
	case K::K4:         return S::SDL_SCANCODE_4;
	case K::K5:         return S::SDL_SCANCODE_5;
	case K::K6:         return S::SDL_SCANCODE_6;
	case K::K7:         return S::SDL_SCANCODE_7;
	case K::K8:         return S::SDL_SCANCODE_8;
	case K::K9:         return S::SDL_SCANCODE_9;
	case K::A:          return S::SDL_SCANCODE_A;
	case K::B:          return S::SDL_SCANCODE_B;
	case K::C:          return S::SDL_SCANCODE_C;
	case K::D:          return S::SDL_SCANCODE_D;
	case K::E:          return S::SDL_SCANCODE_E;
	case K::F:          return S::SDL_SCANCODE_F;
	case K::G:          return S::SDL_SCANCODE_G;
	case K::H:          return S::SDL_SCANCODE_H;
	case K::I:          return S::SDL_SCANCODE_I;
	case K::J:          return S::SDL_SCANCODE_J;
	case K::K:          return S::SDL_SCANCODE_K;
	case K::L:          return S::SDL_SCANCODE_L;
	case K::M:          return S::SDL_SCANCODE_M;
	case K::N:          return S::SDL_SCANCODE_N;
	case K::O:          return S::SDL_SCANCODE_O;
	case K::P:          return S::SDL_SCANCODE_P;
	case K::Q:          return S::SDL_SCANCODE_Q;
	case K::R:          return S::SDL_SCANCODE_R;
	case K::S:          return S::SDL_SCANCODE_S;
	case K::T:          return S::SDL_SCANCODE_T;
	case K::U:          return S::SDL_SCANCODE_U;
	case K::V:          return S::SDL_SCANCODE_V;
	case K::W:          return S::SDL_SCANCODE_W;
	case K::X:          return S::SDL_SCANCODE_X;
	case K::Y:          return S::SDL_SCANCODE_Y;
	case K::Z:          return S::SDL_SCANCODE_Z;
	case K::CAPSLOCK:   return S::SDL_SCANCODE_CAPSLOCK;
	case K::SPACE:      return S::SDL_SCANCODE_SPACE;
	case K::TAB:        return S::SDL_SCANCODE_TAB;
	case K::ENTER:      return S::SDL_SCANCODE_RETURN;
	case K::ESCAPE:     return S::SDL_SCANCODE_ESCAPE;
	case K::BACK:       return S::SDL_SCANCODE_BACKSPACE;
	case K::LWIN:       return S::SDL_SCANCODE_LGUI;
	case K::RWIN:       return S::SDL_SCANCODE_RGUI;
	case K::LCTRL:      return S::SDL_SCANCODE_LCTRL;
	case K::RCTRL:      return S::SDL_SCANCODE_RCTRL;
	case K::LALT:       return S::SDL_SCANCODE_LALT;
	case K::RALT:       return S::SDL_SCANCODE_RALT;
	case K::LSHIFT:     return S::SDL_SCANCODE_LSHIFT;
	case K::RSHIFT:     return S::SDL_SCANCODE_RSHIFT;
	case K::SCROLLLOCK: return S::SDL_SCANCODE_SCROLLLOCK;
	case K::DELETE:     return S::SDL_SCANCODE_DELETE;
	case K::INSERT:     return S::SDL_SCANCODE_INSERT;
	case K::HOME:       return S::SDL_SCANCODE_HOME;
	case K::END:        return S::SDL_SCANCODE_END;
	case K::PAGEUP:     return S::SDL_SCANCODE_PAGEUP;
	case K::PAGEDOWN:   return S::SDL_SCANCODE_PAGEDOWN;
	case K::UP:         return S::SDL_SCANCODE_UP;
	case K::DOWN:       return S::SDL_SCANCODE_DOWN;
	case K::LEFT:       return S::SDL_SCANCODE_LEFT;
	case K::RIGHT:      return S::SDL_SCANCODE_RIGHT;
	case K::NUMLOCK:    return S::SDL_SCANCODE_NUMLOCKCLEAR;
	case K::NP0:        return S::SDL_SCANCODE_KP_0;
	case K::NP1:        return S::SDL_SCANCODE_KP_1;
	case K::NP2:        return S::SDL_SCANCODE_KP_2;
	case K::NP3:        return S::SDL_SCANCODE_KP_3;
	case K::NP4:        return S::SDL_SCANCODE_KP_4;
	case K::NP5:        return S::SDL_SCANCODE_KP_5;
	case K::NP6:        return S::SDL_SCANCODE_KP_6;
	case K::NP7:        return S::SDL_SCANCODE_KP_7;
	case K::NP8:        return S::SDL_SCANCODE_KP_8;
	case K::NP9:        return S::SDL_SCANCODE_KP_9;
	case K::NPADD:      return S::SDL_SCANCODE_KP_PLUS;
	case K::NPSUB:      return S::SDL_SCANCODE_KP_MINUS;
	case K::NPDIV:      return S::SDL_SCANCODE_KP_DIVIDE;
	case K::NPMUL:      return S::SDL_SCANCODE_KP_MULTIPLY;
	case K::NPENTER:    return S::SDL_SCANCODE_KP_ENTER;
	case K::NPPERIOD:   return S::SDL_SCANCODE_KP_PERIOD;
	case K::OEM_1:      return S::SDL_SCANCODE_SEMICOLON;
	case K::OEM_2:      return S::SDL_SCANCODE_SLASH;
	case K::OEM_3:      return S::SDL_SCANCODE_GRAVE;
	case K::OEM_4:      return S::SDL_SCANCODE_LEFTBRACKET;
	case K::OEM_5:      return S::SDL_SCANCODE_BACKSLASH;
	case K::OEM_6:      return S::SDL_SCANCODE_RIGHTBRACKET;
	case K::OEM_7:      return S::SDL_SCANCODE_APOSTROPHE;
	case K::OEM_EQUALS: return S::SDL_SCANCODE_EQUALS;
	case K::OEM_MINUS:  return S::SDL_SCANCODE_MINUS;
	case K::OEM_PERIOD: return S::SDL_SCANCODE_PERIOD;
	case K::OEM_COMMA:  return S::SDL_SCANCODE_COMMA;
	default:            return S::SDL_SCANCODE_UNKNOWN;
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
	int count;
	m_keyboard_state = SDL_GetKeyboardState(&count);

	//for (size_t i = 0; i < count; i++)
	//{
	//	if (m_keyboard_state[i] == 1)
	//	{
	//		auto code = static_cast<SDL_Scancode>( i );
	//		std::println("Pressed {:s}, scancode: {:d}", SDL_GetKeyName(SDL_GetKeyFromScancode(code)), i);
	//	}
	//}
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
