#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	std::vector<std::pair<uint32_t, Window*>> WindowManager::s_Windows;
	WindowHandle WindowManager::s_LastFocusedHandle = { SIZE_MAX, 0 };

	WindowHandle WindowManager::Create(std::string title)
	{
		Window* p = nullptr;
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			p = new OpenGLWindow(title);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}
		
		if (p) {
			for (size_t i = 0; i < s_Windows.size(); i++) {
				if (s_Windows[i].second == nullptr) {
					s_Windows[i].second = p;
					// Generation is incremented when the last was destroyed
					s_LastFocusedHandle = { i, s_Windows[i].first };
					return s_LastFocusedHandle;
				}
			}
			s_LastFocusedHandle = { s_Windows.size(), 1};
			s_Windows.push_back({1, p});
			return s_LastFocusedHandle;
		}

		return { SIZE_MAX, 0 };
	}

	Window* WindowManager::Get(WindowHandle& wh) {
		if (wh.id < s_Windows.size() && s_Windows[wh.id].first == wh.gen) {
			return s_Windows[wh.id].second;
		}

		return nullptr;
	}

	void WindowManager::Destroy(WindowHandle& wh)
	{
		if (wh.id < s_Windows.size() && s_Windows[wh.id].first == wh.gen) {
			delete s_Windows[wh.id].second;
			s_Windows[wh.id].second = nullptr;
			s_Windows[wh.id].first++;
		}
	}

	void WindowManager::OnFocused(Window* winPtr, bool focused)
	{
		if (focused) {
			for (size_t i = 0; i < s_Windows.size(); i++) {
				if (s_Windows[i].second = winPtr) {
					s_LastFocusedHandle = { i, s_Windows[i].first };
					break;
				}
			}
		}
		else if (GetFocused() == winPtr) {
			s_LastFocusedHandle = { SIZE_MAX, 0 };
		}
	}

	Window* WindowManager::GetFocused()
	{
		return Get(s_LastFocusedHandle);
	}
}