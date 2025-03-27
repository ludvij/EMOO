#ifndef GRAPHICS_CLOSE_DIALOG_COMPONENT_HEADER
#define GRAPHICS_CLOSE_DIALOG_COMPONENT_HEADER
#include "IComponent.hpp"
#include <string>

namespace Ui::Component
{
class CloseDialog : public IComponent
{
public:
	CloseDialog(const std::string_view name, const std::string_view msg, bool close=true);
	virtual ~CloseDialog() override;
	void OnCreate() override;
	// Inherited via IComponent
	void OnRender() override;


private:
	std::string m_msg;
	bool m_close{ true };
	int m_frame{ 0 };
};
}
#endif