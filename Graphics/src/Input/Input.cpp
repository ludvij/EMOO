#include "Input.hpp"

void Ui::IInput::ClearActions(Button button)
{
	m_actions[button].clear();
}

void Ui::IInput::ClearActions()
{
	m_actions.clear();
}

void Ui::IInput::AddAction(Button b, const std::function<void()>& action)
{
	m_actions[b].push_back(action);
}

void Ui::IInput::RunActions(Button b)
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

void Ui::IInput::RunActions()
{
	for (const auto& [k, v] : m_actions)
	{
		RunActions(k);
	}
}
