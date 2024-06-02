#ifndef GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER
#define GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER
#include "IComponent.hpp"

#include "Renderer/RendererAPI.hpp"

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
	static constexpr double fade_seconds = 2;
private:

	std::array<uint8_t, 0x10000> m_memory_cache{ };


	char m_text_buffer[5]{ "" };

	ITexture* m_memory{ nullptr };
	ITexture* m_cpu_ram_2{ nullptr };
	ITexture* m_cpu_ram_16{ nullptr };

	ImFont* m_font;


	size_t m_begin = 0x0000;
	int m_scroll_amount = 5;

	bool m_open{ true };
	bool m_16{ true };

	bool m_reset_text_buffer{ true };

	int m_last{ -1 };
	double m_fadeout = 0;

};
}

#endif//GRAPHICS_MEMORY_VIEW_COMPONENT_HEADER