#include "Window.hpp"

void Window::IWindow::AddEventFunction(Event e, const std::function<void(IWindow*, void*)>&& fn)
{
	m_event_fn.insert({ e, fn });
}
