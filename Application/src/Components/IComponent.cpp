#include "IComponent.hpp"

#include <string_view>

namespace Ui::Component
{
IComponent::IComponent(const std::string_view name)
	: name(name)
{
}
void IComponent::OnCreate()
{
}

void IComponent::OnUpdate()
{
}
/*
* x
* ↓
* 1 . . . . . . . # # #
*   . . . . . . # # . .
*   . . . . . # # . . .
*   . . . . . # . . . .
*   . . . . . # . . . .
*   . . . . # . . . . .
*   . . . . # . . . . .
*   . . . # # . . . . .
*   . . # # . . . . . .
* 0 # # # . . . . . . .
*   m                 M ← t
*/
float EaseInOutCubic(float x, const float m, const float M)
{
	x = ( x - m ) / M;

	if (x < .5)
	{
		return 4.0f * x * x * x;
	}
	else
	{
		const float a = ( -2.0f * x + 2.0f );
		return 1 - a * a * a / 2.0f;
	}
};

bool operator==(const IComponent& lhs, const IComponent& rhs)
{
	return lhs.name == rhs.name;
}


bool IComponent::operator==(const std::string_view other_name) const
{
	return this->name == other_name;
}
}
