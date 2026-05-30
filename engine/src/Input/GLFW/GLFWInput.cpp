#include "pch.h"

#include "GLFWInput.h"

namespace NXTN {
	bool GLFWInput::IsKeyPressedImpl(KeyCode keycode)
	{
		int state = glfwGetKey(
			(GLFWwindow*)(WindowManager::GetFocused()->GetNativeWindow()),
			MapToGLFWKeyCode(keycode)
		);
		
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressedImpl(MouseButtonCode button)
	{
		int state = glfwGetMouseButton(
			(GLFWwindow*)(WindowManager::GetFocused()->GetNativeWindow()),
			MapToGLFWMouseButton(button)
		);

		return state == GLFW_PRESS;
	}

	vec2 GLFWInput::GetMousePosImpl()
	{
		double resX, resY;
		glfwGetCursorPos(
			(GLFWwindow*)(WindowManager::GetFocused()->GetNativeWindow()),
			&resX,
			&resY
		);

		return vec2(resX, resY);
	}
}