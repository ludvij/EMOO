#include "IComponent.hpp"

namespace Ui::Component
{


void IComponent::OnCreate()
{
}

void IComponent::OnUpdate()
{
}

bool operator==(const IComponent& lhs, const IComponent& rhs)
{
	return lhs.id == rhs.id;
}


bool IComponent::operator==(const UUID& id) const
{
	return this->id == id;
}
}
