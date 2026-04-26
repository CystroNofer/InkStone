#pragma once

#include "Input/Input.h"

#include <GLFW/glfw3.h>

namespace NXTN {
	class GLFWInput : public Input
	{
	public:
		GLFWInput(void* windowPtr);

	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual vec2 GetMousePosImpl() override;

	private:
		static GLFWwindow* m_Window;

		static std::unordered_map<int, int> keyCodeMapToGLFW;
	};
}
