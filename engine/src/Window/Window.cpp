#include "pch.h"

#include "OpenGL/OpenGLWindow.h"

namespace NXTN {
	Window* Window::Create(std::string title)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return new OpenGLWindow(title);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}
}