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

bool operator==(const IComponent& lhs, const IComponent& rhs)
{
	return lhs.name == rhs.name;
}


bool IComponent::operator==(const std::string_view name) const
{
	return this->name == name;
}
}
