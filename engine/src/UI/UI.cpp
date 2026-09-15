#include "pch.h"

#include "UI.h"

#include "Time.h"
#include "Window/Window.h"
#include "Rendering/GraphicsDevice.h"

namespace NXTN {
	std::unique_ptr<UI> UI::s_UI = nullptr;

	void UI::Init(unsigned int w, unsigned int h)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return;
		}
		s_UI.reset(device->CreateUI(w, h));
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
