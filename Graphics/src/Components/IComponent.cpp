#include "IComponent.hpp"

namespace Ui::Component
{
bool operator==(const IComponent& lhs, const IComponent& rhs)
{
	return lhs.id == rhs.id;
}


void IComponent::OnCreate()
{
}

bool IComponent::WasRemoved() const
{
	return m_removed;
}
bool IComponent::operator==(const UUID& id) const
{
	return this->id == id;
}
}
