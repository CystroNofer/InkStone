#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	HandleMap<Window> WindowManager::s_Windows;
	Handle<Window> WindowManager::s_LastFocusedHandle = { SIZE_MAX, 0 };

	Handle<Window> WindowManager::Create(std::string title)
	{
		Window* p;
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			p = (Window*)(new OpenGLWindow(title));
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}
		
		if (p) {
			s_LastFocusedHandle = s_Windows.Add(p);

			p->SetFocusedCallback([](bool focused) {
				OnFocused(s_LastFocusedHandle, focused);
			});

			return s_LastFocusedHandle;
		}

		return { SIZE_MAX, 0 };
	}

	Window* WindowManager::Get(Handle<Window>& wh) {
		return s_Windows.Get(wh);
	}

	void WindowManager::Destroy(Handle<Window>& wh)
	{
		s_Windows.Remove(wh);
	}

	void WindowManager::OnFocused(Handle<Window> wh, bool focused)
	{
		if (focused && s_Windows.Validate(wh)) {
			s_LastFocusedHandle = wh;
		}
		else if (s_LastFocusedHandle == wh) {
			s_LastFocusedHandle = { SIZE_MAX, 0 };
		}
	}

	Window* WindowManager::GetFocused()
	{
		return Get(s_LastFocusedHandle);
	}
}