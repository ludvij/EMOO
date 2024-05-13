#ifndef GRAPHICS_INPUT_HEADER
#define GRAPHICS_INPUT_HEADER
#include <deque>
#include <functional>
#include <unordered_map>

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

class IInput
{
public:
	virtual ~IInput() = default;
	virtual bool GetButton(Button b) = 0;

	void ClearActions(Button button)
	{
		m_actions[button].clear();
	}
	void ClearActions()
	{
		m_actions.clear();
	}

	void AddAction(Button b, const std::function<void()>& action)
	{
		m_actions[b].push_back(action);
	}

	void RunActions(Button b)
	{
		if (!GetButton(b))
		{
			return;
		}

		for (const auto& action : m_actions[b])
		{
			action();
		}
	}
	void RunActions()
	{
		for (const auto& [k, v] : m_actions)
		{
			RunActions(k);
		}
	}

protected:
	std::unordered_map<Button, std::deque<std::function<void()>>> m_actions;

};
}
#endif