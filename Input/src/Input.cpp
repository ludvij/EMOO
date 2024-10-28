#include "Input.hpp"

#include <ranges>


namespace Input
{

static constexpr int BUTTON_BEGIN = Button::BUTTON_NONE;
static constexpr int BUTTON_END = Button::BUTTON_MISC;
static constexpr int KEY_BEGIN = Key::KEY_NONE;
static constexpr int KEY_END = Key::OEM_COMMA;

static constexpr int MODIFIER_BEGIN = Key::LWIN;
static constexpr int MODIFIER_END = Key::SHIFT;

static constexpr bool is_button(int trigger)
{
	return trigger >= BUTTON_BEGIN && trigger <= BUTTON_END;
}

static constexpr bool is_key(int trigger)
{
	return trigger >= KEY_BEGIN && trigger <= KEY_END;
}

static constexpr bool is_modifier(int trigger)
{
	return trigger >= MODIFIER_BEGIN && trigger <= MODIFIER_END;
}


auto GetButtonName(Button b)
{
	switch (b)
	{
	case Input::Button::FACE_DOWN:   return "FACE_DOWN";
	case Input::Button::FACE_LEFT:   return "FACE_LEFT";
	case Input::Button::FACE_UP:     return "FACE_UP";
	case Input::Button::FACE_RIGHT:  return "FACE_RIGHT";
	case Input::Button::DPAD_UP:     return "DPAD_UP";
	case Input::Button::DPAD_DOWN:   return "DPAD_DOWN";
	case Input::Button::DPAD_LEFT:   return "DPAD_LEFT";
	case Input::Button::DPAD_RIGHT:  return "DPAD_RIGHT";
	case Input::Button::START:       return "START";
	case Input::Button::SELECT:      return "SELECT";
	case Input::Button::R1:          return "R1";
	case Input::Button::R3:          return "R3";
	case Input::Button::L1:          return "L1";
	case Input::Button::L3:          return "L3";
	case Input::Button::BUTTON_MISC: return "MISC";
	default:                         return "INVALID";
	}
}

auto GetKeyName(Key k)
{
	switch (k)
	{
	case Input::Key::KEY_NONE:       return "NONE";
	case Input::Key::F1:             return "F1";
	case Input::Key::F2:             return "F2";
	case Input::Key::F3:             return "F3";
	case Input::Key::F4:             return "F4";
	case Input::Key::F5:             return "F5";
	case Input::Key::F6:             return "F6";
	case Input::Key::F7:             return "F7";
	case Input::Key::F8:             return "F8";
	case Input::Key::F9:             return "F9";
	case Input::Key::F10:            return "F10";
	case Input::Key::F11:            return "F11";
	case Input::Key::F12:            return "F12";
	case Input::Key::F13:            return "F13";
	case Input::Key::F14:            return "F14";
	case Input::Key::F15:            return "F15";
	case Input::Key::F16:            return "F16";
	case Input::Key::F17:            return "F17";
	case Input::Key::F18:            return "F18";
	case Input::Key::F19:            return "F19";
	case Input::Key::F20:            return "F20";
	case Input::Key::F21:            return "F21";
	case Input::Key::F22:            return "F22";
	case Input::Key::F23:            return "F23";
	case Input::Key::F24:            return "F24";
	case Input::Key::K0:             return "0";
	case Input::Key::K1:             return "1";
	case Input::Key::K2:             return "2";
	case Input::Key::K3:             return "3";
	case Input::Key::K4:             return "4";
	case Input::Key::K5:             return "5";
	case Input::Key::K6:             return "6";
	case Input::Key::K7:             return "7";
	case Input::Key::K8:             return "8";
	case Input::Key::K9:             return "9";
	case Input::Key::A:              return "A";
	case Input::Key::B:              return "B";
	case Input::Key::C:              return "C";
	case Input::Key::D:              return "D";
	case Input::Key::E:              return "E";
	case Input::Key::F:              return "F";
	case Input::Key::G:              return "G";
	case Input::Key::H:              return "H";
	case Input::Key::I:              return "I";
	case Input::Key::J:              return "J";
	case Input::Key::K:              return "K";
	case Input::Key::L:              return "L";
	case Input::Key::M:              return "M";
	case Input::Key::N:              return "N";
	case Input::Key::O:              return "O";
	case Input::Key::P:              return "P";
	case Input::Key::Q:              return "Q";
	case Input::Key::R:              return "R";
	case Input::Key::S:              return "S";
	case Input::Key::T:              return "T";
	case Input::Key::U:              return "U";
	case Input::Key::V:              return "V";
	case Input::Key::W:              return "W";
	case Input::Key::X:              return "X";
	case Input::Key::Y:              return "Y";
	case Input::Key::Z:              return "Z";
	case Input::Key::CAPSLOCK:       return "CAPSLOCK";
	case Input::Key::SPACE:          return "SPACE";
	case Input::Key::TAB:            return "TAB";
	case Input::Key::ENTER:          return "ENTER";
	case Input::Key::ESCAPE:         return "ESCAPE";
	case Input::Key::BACK:           return "BACK";
	case Input::Key::LWIN:           return "LWIN";
	case Input::Key::RWIN:           return "RWIN";
	case Input::Key::LCTRL:          return "LCTRL";
	case Input::Key::RCTRL:          return "RCTRL";
	case Input::Key::LALT:           return "LALT";
	case Input::Key::RALT:           return "RALT";
	case Input::Key::LSHIFT:         return "LSHIFT";
	case Input::Key::RSHIFT:         return "RSHIFT";
	case Input::Key::SCROLLLOCK:     return "SCROLLLOCK";
	case Input::Key::DELETE:         return "DELETE";
	case Input::Key::INSERT:         return "INSERT";
	case Input::Key::HOME:           return "HOME";
	case Input::Key::END:            return "END";
	case Input::Key::PAGEUP:         return "PAGEUP";
	case Input::Key::PAGEDOWN:       return "PAGEDOWN";
	case Input::Key::UP:             return "UP";
	case Input::Key::DOWN:           return "DOWN";
	case Input::Key::LEFT:           return "LEFT";
	case Input::Key::RIGHT:          return "RIGHT";
	case Input::Key::NUMLOCK:        return "NUMLOCK";
	case Input::Key::NP0:            return "NP0";
	case Input::Key::NP1:            return "NP1";
	case Input::Key::NP2:            return "NP2";
	case Input::Key::NP3:            return "NP3";
	case Input::Key::NP4:            return "NP4";
	case Input::Key::NP5:            return "NP5";
	case Input::Key::NP6:            return "NP6";
	case Input::Key::NP7:            return "NP7";
	case Input::Key::NP8:            return "NP8";
	case Input::Key::NP9:            return "NP9";
	case Input::Key::NPADD:          return "NPADD";
	case Input::Key::NPSUB:          return "NPSUB";
	case Input::Key::NPDIV:          return "NPDIV";
	case Input::Key::NPMUL:          return "NPMUL";
	case Input::Key::NPENTER:        return "NPENTER";
	case Input::Key::NPPERIOD:       return "NPPERIOD";
	case Input::Key::OEM_1:          return "OEM1";
	case Input::Key::OEM_2:          return "OEM2";
	case Input::Key::OEM_3:          return "OEM3";
	case Input::Key::OEM_4:          return "OEM4";
	case Input::Key::OEM_5:          return "OEM5";
	case Input::Key::OEM_6:          return "OEM6";
	case Input::Key::OEM_7:          return "OEM7";
	case Input::Key::OEM_MINUS:      return "OEMMINUS";
	case Input::Key::OEM_EQUALS:     return "OEMEQUALS";
	case Input::Key::OEM_COMMA:      return "OEMCOMMA";
	case Input::Key::OEM_PERIOD:     return "OEMPERIOD";
	case Input::Key::CTRL:           return "CTRL";
	case Input::Key::SHIFT:          return "SHIFT";
	case Input::Key::WIN:            return "WIN";
	case Input::Key::ALT:            return "ALT";
	default:                         return "INVALID";
	}
}

static constexpr inline auto enum_range = [](auto front, auto back)
	{
		return std::views::iota(std::to_underlying(front), std::to_underlying(back) + 1)
			| std::views::transform([](auto e)
				{
					return decltype( front )( e );
				});
	};



bool IInput::IsKeyModified(std::initializer_list<Key> modifiers/*={}*/) const
{
	if (modifiers.size() == 0)
	{
		for (const auto mod : enum_range(Key::LWIN, Key::SHIFT))
		{
			if (GetKey(mod)) return true;
		}
	}
	else
	{
		for (const auto mod : modifiers)
		{
			if (GetKey(mod)) return true;
		}
	}
	return false;
}


bool IInput::IsRepeating(int trigger) const
{
	return m_pressed.contains(trigger);
}

bool IInput::CanRepeatAfter(const ms_t ms, int trigger) const
{
	if (trigger < 0)
	{
		trigger = m_current;
	}
	if (m_pressed.contains(trigger))
	{
		const auto now = std::chrono::steady_clock::now();
		const auto diff = std::chrono::duration_cast<ms_t>( now - m_pressed.at(trigger) );
		return ms < diff;
	}
	return true;
}

bool IInput::CanRepeatEvery(ms_t ms, int trigger)
{
	if (trigger < 0)
	{
		trigger = m_current;
	}
	if (m_pressed.contains(trigger))
	{
		if (!m_time_repetitions.contains(m_current_action))
		{
			m_time_repetitions.insert({ m_current_action, 0 });
		}
		auto reps = m_time_repetitions.at(m_current_action);
		const auto now = std::chrono::steady_clock::now();
		const auto diff = std::chrono::duration_cast<ms_t>( now - m_pressed.at(trigger) );
		const size_t actual_diffs = diff / ms;
		if (actual_diffs != reps)
		{
			m_time_repetitions[m_current_action] = actual_diffs;
			return true;
		}
		return false;
	}
	return true;
}

void IInput::ClearActions(int trigger)
{
	if (trigger < 0)
	{
		m_actions.clear();
	}
	else
	{
		m_actions[trigger].clear();
	}
}


void IInput::AddAction(int trigger, const std::function<void(IInput*)>& action)
{
	m_actions[trigger].push_back(action);
	m_time_repetitions.clear();
}

void IInput::RunActions(int trigger)
{

	if (!is_pressed(trigger))
	{
		return;
	}
	m_current = trigger;
	for (const auto& action : m_actions[trigger])
	{
		action(this);
		m_current_action++;
	}
}

void IInput::RunActions()
{
	m_current_action = 0;
	for (const auto& [k, v] : m_actions)
	{
		RunActions(k);
	}
}

void IInput::Update()
{
	platform_override();

	RunActions();
	update_gamepad_state();
	update_keyboard_state();
}

void IInput::update_keyboard_state()
{
	for (const auto key : enum_range(Key::F1, Key::OEM_COMMA))
	{
		if (GetKey(key) && !m_pressed.contains(key))
		{
			m_pressed.insert({ key, std::chrono::steady_clock::now() });
		}
		if (!GetKey(key) && m_pressed.contains(key))
		{
			m_pressed.erase(key);
		}
	}
}

void IInput::update_gamepad_state()
{
	for (const auto button : enum_range(Button::FACE_DOWN, Button::BUTTON_MISC))
	{
		if (GetButton(button) && !m_pressed.contains(button))
		{
			m_pressed.insert({ button, std::chrono::steady_clock::now() });
		}
		if (!GetButton(button) && m_pressed.contains(button))
		{
			m_pressed.erase(button);
		}
	}
}
bool IInput::is_pressed(int trigger) const
{
	if (is_key(trigger))
	{
		return GetKey(trigger);
	}
	else if (is_button(trigger))
	{
		return GetButton(trigger);
	}
	return false;
}
}