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

	private:
	};
}
