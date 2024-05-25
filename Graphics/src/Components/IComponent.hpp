#ifndef GRAPHICS_ICOMPONENT_HEADER
#define GRAPHICS_ICOMPONENT_HEADER

#include <lud_id.hpp>
#include <string_view>

namespace Ui::Component
{
class IComponent
{
public:
	IComponent(const std::string_view name);
	virtual ~IComponent() = default;

	virtual void OnCreate();
	virtual void OnRender() = 0;
	virtual void OnUpdate();

	friend bool operator==(const IComponent& lhs, const IComponent& rhs);
	bool operator==(const std::string_view id) const;


public:
	bool removed{ false };
	std::string_view name;
protected:

};
}

#endif