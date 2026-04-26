#pragma once

#include "Core.h"
#include "Log.h"

#include "KeyCode.h"

#include "Math/Math.h"

namespace NXTN {
	class Input
	{
	public:
		static void Init(void* windowPtr);

		static inline bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		static inline bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		static inline vec2 GetMousePos() { return s_Instance->GetMousePosImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual vec2 GetMousePosImpl() = 0;

	private:
		static Input* s_Instance;
	};
}
