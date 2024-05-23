#ifndef GRAPHICS_INPUT_HEADER
#define GRAPHICS_INPUT_HEADER
#include <deque>
#include <functional>
#include <unordered_map>

#include <print>

namespace Ui
{
enum class Button
{
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

inline auto GetButtonName(Button b)
{
	switch (b)
	{
	case Ui::Button::FACE_DOWN:  return "FACE_DOWN";
	case Ui::Button::FACE_LEFT:  return "FACE_LEFT";
	case Ui::Button::FACE_UP:    return "FACE_UP";
	case Ui::Button::FACE_RIGHT: return "FACE_RIGHT";
	case Ui::Button::DPAD_UP:    return "DPAD_UP";
	case Ui::Button::DPAD_DOWN:  return "DPAD_DOWN";
	case Ui::Button::DPAD_LEFT:  return "DPAD_LEFT";
	case Ui::Button::DPAD_RIGHT: return "DPAD_RIGHT";
	case Ui::Button::START:      return "START";
	case Ui::Button::SELECT:     return "SELECT";
	case Ui::Button::R1:         return "R1";
	case Ui::Button::R3:         return "R3";
	case Ui::Button::L1:         return "L1";
	case Ui::Button::L3:         return "L3";
	case Ui::Button::MISC:       return "MISC";
	default:                     return "INVALID";
	}
}

class IInput
{
public:
	virtual ~IInput() = default;
	virtual bool GetButton(Button b) = 0;

	void ClearActions(Button button);
	void ClearActions();

	void AddAction(Button b, const std::function<void()>& action);

	void RunActions(Button b);
	void RunActions();

	virtual void ProcessEvents(void* event) = 0;

protected:
	std::unordered_map<Button, std::deque<std::function<void()>>> m_actions;

};
}
#endif