#ifndef GRAPHICS_INPUT_HEADER
#define GRAPHICS_INPUT_HEADER
#include <deque>
#include <functional>
#include <unordered_map>

#include <chrono>

#include <print>

#define INPUT_NOT_REPEATED(x) if (x->IsRepeating()) return
#define INPUT_KEY_NOT_MODIFIED(x) if (x->IsKeyModified()) return
#define INPUT_KEY_MODIFIED(x, ...) if (!x->IsKeyModified({__VA_ARGS__})) return

namespace Input
{
enum class Button
{
	NONE,
	FACE_DOWN,
	FACE_LEFT,
	FACE_UP,
	FACE_RIGHT,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	START,
	SELECT,
	R1,
	//R2, this is an axis
	R3,
	L1,
	//L2, this is an axis
	L3,
	MISC,
};

enum class Key
{
	NONE,
	// FUNCTION KEYS
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
	// NORMAL KEYS
	K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	// GENERAL KEYS
	CAPSLOCK, SPACE, TAB, ENTER, ESCAPE, BACK,
	// MODIFIERS
	LWIN, RWIN, LCTRL, RCTRL, LALT, RALT, LSHIFT, RSHIFT,
	// CURSOR CONTROL
	SCROLLLOCK, DELETE, INSERT, HOME, END, PAGEUP, PAGEDOWN, UP, DOWN, LEFT, RIGHT,
	// NUMPAD
	NUMLOCK, NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
	NPADD, NPSUB, NPDIV, NPMUL, NPENTER, NPPERIOD,
	// werid thing beside keys
	// some headache keys
	GRAVE,
};

auto GetButtonName(Button b);
auto GetKeyName(Key k);

class IInput
{
public:

	virtual ~IInput() = default;
	virtual bool GetButton(Button b) const = 0;
	virtual bool GetKey(Key k) const = 0;

	// an empty mod list will test all modifiers
	bool IsKeyModified(std::initializer_list<Key> modifiers={}) const;

	bool IsRepeating() const;
	bool IsKeyRepeating(Key k) const;
	bool IsButtonRepeating(Button b) const;

	void ClearActions(Button button);
	void ClearActions();

	void AddGamepadAction(Button b, const std::function<void(IInput*)>& action);
	void AddKeyboardAction(Key b, const std::function<void(IInput*)>& action);

	void RunGamepadActions(Button b);
	void RunKeyboardActions(Key b);
	void RunActions();
	void Update();

	virtual void ProcessEvents(void* event) = 0;

protected:
	virtual void platform_override();

	void update_keyboard_state();
	void update_gamepad_state();

protected:
	std::unordered_map<Button, std::deque<std::function<void(IInput*)>>> m_button_actions;
	std::unordered_map<Key, std::deque<std::function<void(IInput*)>>> m_key_actions;

	Button m_current_button;
	Key m_current_key;

	std::unordered_map<Button, std::chrono::time_point<std::chrono::steady_clock>> m_pressed_buttons;
	std::unordered_map<Key, std::chrono::time_point<std::chrono::steady_clock>> m_pressed_keys;

};
}
#endif