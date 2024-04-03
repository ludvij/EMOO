#ifndef GRAPHICS_ICOMPONENT_HEADER
#define GRAPHICS_ICOMPONENT_HEADER

namespace Ui 
{
class IComponent
{
public:
	virtual ~IComponent() = default;

	virtual void OnCreate() {};
	virtual void OnRender() = 0; 
};
}

#endif