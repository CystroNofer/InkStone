#pragma once

#include "Window/Window.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

namespace NXTN {
	class OpenGLWindow : public Window
	{
	public:
		OpenGLWindow(std::string title, bool vSync = false);
		virtual ~OpenGLWindow();

		virtual void Update() override;

		virtual inline unsigned int GetWidth() const override { return m_WinData.width; }
		virtual inline unsigned int GetHeight() const override { return m_WinData.height; }
		virtual inline bool IsVSync() const override { return m_WinData.vSync; }
		virtual void SetVSync(bool enabled) override;

		//virtual void SetEventCallback(const EventCallbackFn& callback) override { m_WinData.EventCallback = callback; }

		virtual inline void* GetNativeWindow() const override { return m_Window; }

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			int width = 0, height = 0;
			std::string title;
			bool vSync = false;

			//EventCallbackFn EventCallback;
		};

		WindowData m_WinData;
	};
}

