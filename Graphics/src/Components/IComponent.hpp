#ifndef GRAPHICS_ICOMPONENT_HEADER
#define GRAPHICS_ICOMPONENT_HEADER

#include "UUID/UUID.hpp"

namespace Ui::Component
{
class IComponent
{
public:
	virtual ~IComponent() = default;

	virtual void OnCreate();
	virtual void OnRender() = 0;
	virtual void OnUpdate();

	friend bool operator==(const IComponent& lhs, const IComponent& rhs);
	bool operator==(const UUID& id) const;


public:
	UUID id;
	bool removed{ false };
protected:

};
}

#endif