#pragma once

#include "Input/Input.h"
#include "GLFWKeyMap.h"

#include <GLFW/glfw3.h>

namespace NXTN {
	class GLFWInput : public Input
	{
	public:
		GLFWInput() {}

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) override;
		virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) override;
		virtual vec2 GetMousePosImpl() override;
		virtual vec2 GetMouseMovementImpl() override;
		virtual void UpdateImpl() override;

	private:
		float m_LastCursorPosX = 0.0f, m_LastCursorPosY = 0.0f;
		float m_CursorDiffX = 0.0f, m_CursorDiffY = 0.0f;
		GLFWwindow* m_Window = nullptr;
	};
}
