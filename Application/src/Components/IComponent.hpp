#ifndef GRAPHICS_ICOMPONENT_HEADER
#define GRAPHICS_ICOMPONENT_HEADER

#include <lud_id.hpp>
#include <string_view>

#define DEFINE_COLOR(R, G, B) \
std::endian::native == std::endian::little ? 0xFF << 24 | B << 16 | G << 8 | R : R << 24 | G << 16 | B << 8 | 0xFF

#define DEFINE_COLOR_A(R, G, B, A) \
std::endian::native == std::endian::little ? A << 24 | B << 16 | G << 8 | R : R << 24 | G << 16 | B << 8 | A


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