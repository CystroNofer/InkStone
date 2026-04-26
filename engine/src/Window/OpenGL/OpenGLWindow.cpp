#include "pch.h"

#include "OpenGLWindow.h"

#include "Input/GLFW/GLFWKeyMap.h"

namespace NXTN {
	static bool s_GLFWInitialized = false;

	OpenGLWindow::OpenGLWindow(std::string title, bool vSync)
	{
		vSync = vSync;
		//EventCallback = [](Event&) {};

		// GLFW initialization
		if (glfwInit())
		{
			Log::Info("GLFW initialized");
		}
		else
		{
			Log::Error("Failed to initialize GLFW");
			NXTN_ERROR;
		}
		// Create window and context
		m_Window = glfwCreateWindow(800, 600, title.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			Log::Error("Failed to create window (Title: %s)", title.c_str());
			NXTN_ERROR;
		}
		glfwMakeContextCurrent(m_Window);

		glfwMaximizeWindow(m_Window);
		glfwGetWindowSize(m_Window, &m_WinData.width, &m_WinData.height);

		glfwSetWindowUserPointer(m_Window, &m_WinData);

		// GLAD initialization
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::Info("GLAD initialized");
		}
		else
		{
			Log::Error("Failed to initialize GLAD");
			NXTN_ERROR;
		}

		glViewport(0, 0, m_WinData.width, m_WinData.height);

		// Log context information
		Log::Info("OpenGL context initialized");
		Log::Info("  ©À- Hardware Info:  %s", glGetString(GL_RENDERER));
		Log::Info("  ©¸- OpenGL Version: %s", glGetString(GL_VERSION));

		// V Sync
		glfwSwapInterval(vSync ? 1 : 0);

		// GLFW event callbacks
		// Window close
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				EventBuffer::PushEvent(new WindowCloseEvent());
			}
		);
		// Window resize
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				windowData.width = width;
				windowData.height = height;

				EventBuffer::PushEvent(new WindowResizeEvent(width, height));
			}
		);
		// Key press
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scanCode, int action, int mods)
			{
				//WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				key = keycodeMapFromGLFW[key];

				switch (action)
				{
				case GLFW_PRESS:
					EventBuffer::PushEvent(new KeyPressEvent(key, 0));
					break;
				case GLFW_RELEASE:
					EventBuffer::PushEvent(new KeyReleaseEvent(key));
					break;
				case GLFW_REPEAT:
					EventBuffer::PushEvent(new KeyPressEvent(key, 1));
					break;
				}
			}
		);
		// Mouse button press
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int key, int action, int mods)
			{
				//WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				key = keycodeMapFromGLFW[key];

				switch (action)
				{
				case GLFW_PRESS:
					EventBuffer::PushEvent(new MouseButtonPressEvent(key));
					break;
				case GLFW_RELEASE:
					EventBuffer::PushEvent(new MouseButtonReleaseEvent(key));
					break;
				}
			}
		);
		// Mouse scroll
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				//WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				EventBuffer::PushEvent(new MouseScrollEvent((float)xOffset, (float)yOffset));
			}
		);
		// Cursor move
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				//WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				EventBuffer::PushEvent(new MouseMoveEvent((float)xPos, (float)yPos));
			}
		);
	}

	OpenGLWindow::~OpenGLWindow()
	{
		glfwDestroyWindow(m_Window);
	}

	void OpenGLWindow::Update()
	{
		NXTN_PROFILE_FUNCTION()

		glfwPollEvents();

		glfwSwapBuffers(m_Window);
	}

	void OpenGLWindow::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);

		m_WinData.vSync = enabled;
	}
}
