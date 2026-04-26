#include "pch.h"

#include "OpenGL/OpenGLUI.h"

namespace NXTN {
	std::unique_ptr<UI> UI::s_UI = nullptr;

	void UI::Init(const std::shared_ptr<Window>& window)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			s_UI.reset(new OpenGLUI(window));
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return;
	}

	void UI::NewFrame()
	{
		NXTN_PROFILE_FUNCTION()

		if (!s_UI)
		{
			Log::Warning("UI uninitialized");
			return;
		}
		s_UI->NewFrameImpl();
	}

	void UI::EndFrame()
	{
		NXTN_PROFILE_FUNCTION()

		if (!s_UI)
		{
			Log::Warning("UI uninitialized");
			return;
		}
		s_UI->EndFrameImpl();
	}

	void UI::OnEvent(Event*& event_ptr)
	{
		if (!s_UI)
		{
			Log::Warning("UI uninitialized");
			return;
		}
		s_UI->OnEventImpl(event_ptr);
	}
}
