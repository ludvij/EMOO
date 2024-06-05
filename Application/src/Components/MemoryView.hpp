#ifndef GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER
#define GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER
#include "IComponent.hpp"

#include <RendererAPI.hpp>

#include <array>

namespace Ui::Component
{
class MemoryView : public IComponent
{
public:

	MemoryView(const std::string_view name, ImFont* font);
	virtual ~MemoryView() override;

	// Inherited via IComponent
	virtual void OnRender() override;
	virtual void OnCreate() override;
	virtual void OnUpdate() override;

private:
	void repr_cpu_ram_16();
	void repr_cpu_ram_2();

public:
	static constexpr float fade_seconds = 3;
private:

	std::array<uint8_t, 0x10000> m_memory_cache{ };


	char m_text_buffer[5]{ "" };

	Renderer::ITexture* m_memory{ nullptr };
	Renderer::ITexture* m_cpu_ram_2{ nullptr };
	Renderer::ITexture* m_cpu_ram_16{ nullptr };

	ImFont* m_font;


	size_t m_begin = 0x0000;
	int m_scroll_amount = 5;

	bool m_open{ true };
	bool m_16{ true };

	bool m_reset_text_buffer{ true };

	int m_last{ -1 };
	float m_fadeout = 0;

	bool m_first_popup_frame{ true };

};
}

#endif//GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER