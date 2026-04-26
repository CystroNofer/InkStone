#pragma once

#include "UI/UI.h"

#include "ImGuiBackend/imgui_impl_glfw.h"
#include "ImGuiBackend/imgui_impl_opengl3.h"

namespace NXTN {
	class OpenGLUI : public UI
	{
	public:
		OpenGLUI(const std::shared_ptr<Window>& window);
		~OpenGLUI() {}

		virtual void NewFrameImpl() override;
		virtual void EndFrameImpl() override;

		virtual void OnEventImpl(Event*& event_ptr) override;

	private:
		const std::shared_ptr<Window>& m_Window;
	};
}