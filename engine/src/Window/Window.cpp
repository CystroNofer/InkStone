#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	HandleMap<Window> WindowManager::s_Windows;
	Handle<Window> WindowManager::s_LastFocusedHandle = { SIZE_MAX, 0 };

	Handle<Window> WindowManager::Create(std::string title)
	{
		Window* p = nullptr;
		switch (APISetting::GetWindowAPI())
		{
		case WindowAPI::None:
			Log::Error("No window API specified");
			break;
		case WindowAPI::GLFW:
			if (APISetting::GetGraphicsAPI() == GraphicsAPI::OpenGL)
			{
				p = new OpenGLWindow(title);
			}
			else
			{
				Log::Error("Unsupported graphics API for GLFW window");
			}
			break;
		default:
			Log::Error("Unsupported window API");
			break;
		}
		
		if (p) {
			Handle<Window> handle = s_Windows.Add(p);
			s_LastFocusedHandle = handle;

			p->SetFocusedCallback([handle](bool focused) {
				OnFocused(handle, focused);
			});

			return handle;
		}

		return Handle<Window>();
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
