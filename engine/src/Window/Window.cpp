#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	std::vector<std::pair<uint32_t, Window*>> WindowManager::s_Windows;
	size_t WindowManager::s_LastFocusedID = 0;

	WindowHandle WindowManager::Create(std::string title)
	{
		// i will be the slot id for the window
		size_t i = 0;
		bool needNew = true;
		for (; i < s_Windows.size(); i++) {
			if (s_Windows[i].second = nullptr) {
				// Generation is incremented when the last was destroyed
				needNew = false;
				break;
			}
		}
		if (needNew) {
			s_Windows.push_back({ 0, nullptr });
		}

		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			s_Windows[i].second = new OpenGLWindow(i, title);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return { i, s_Windows[i].first };
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

	void WindowManager::OnFocused(size_t id)
	{
		s_LastFocusedID = id;
	}

	Window* WindowManager::GetFocused()
	{
		if (s_LastFocusedID < s_Windows.size())
		{
			return s_Windows[s_LastFocusedID].second;
		}
		return nullptr;
	}
}