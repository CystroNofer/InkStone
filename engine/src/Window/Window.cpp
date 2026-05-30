#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	std::vector<std::pair<uint32_t, Window*>> WindowManager::m_Windows;

	WindowHandle WindowManager::Create(std::string title)
	{
		Window* winPtr = nullptr;

		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			winPtr = new OpenGLWindow(title);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		if (winPtr) {
			for (size_t i = 0; i < m_Windows.size(); i++) {
				if (m_Windows[i].second = nullptr) {
					m_Windows[i].second = winPtr;
					// Generation is incremented when the last was destroyed
					return { i, m_Windows[i].first };
				}
			}

			m_Windows.push_back({ 1, winPtr });
			return { m_Windows.size() - 1, 1 };
		}

		return { SIZE_MAX, 0 };
	}

	Window* WindowManager::Get(WindowHandle& wh) {
		if (wh.id < m_Windows.size() && m_Windows[wh.id].first == wh.gen) {
			return m_Windows[wh.id].second;
		}

		return nullptr;
	}

	void WindowManager::Destroy(WindowHandle& wh)
	{
		if (wh.id < m_Windows.size() && m_Windows[wh.id].first == wh.gen) {
			delete m_Windows[wh.id].second;
			m_Windows[wh.id].first++;
		}
	}
}