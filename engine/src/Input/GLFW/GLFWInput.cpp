#include "pch.h"

#include "GLFWInput.h"

namespace NXTN {
	bool GLFWInput::IsKeyPressedImpl(KeyCode keycode)
	{
		if (!m_Window) return false;

		int state = glfwGetKey(m_Window, MapToGLFWKeyCode(keycode));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressedImpl(MouseButtonCode button)
	{
		if (!m_Window) return false;

		int state = glfwGetMouseButton(m_Window, MapToGLFWMouseButton(button));

		return state == GLFW_PRESS;
	}

	vec2 GLFWInput::GetMousePosImpl()
	{
		if (!m_Window) return vec2(m_LastCursorPosX, m_LastCursorPosY);

		double resX, resY;
		glfwGetCursorPos(m_Window, &resX, &resY);

		return vec2(resX, resY);
	}

	vec2 GLFWInput::GetMouseMovementImpl()
	{
		return vec2(m_CursorDiffX, m_CursorDiffY);
	}

	void GLFWInput::UpdateImpl()
	{
		glfwPollEvents();

		Window* winPtr = WindowManager::GetFocused();
		if (winPtr) m_Window = (GLFWwindow*)(winPtr->GetNativeWindow());

		if (m_Window) {
			double x, y;
			glfwGetCursorPos(m_Window, &x, &y);

			m_CursorDiffX = x - m_LastCursorPosX;
			m_CursorDiffY = y - m_LastCursorPosY;
			m_LastCursorPosX = x;
			m_LastCursorPosY = y;
		}
	}
}