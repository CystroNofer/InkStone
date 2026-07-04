#pragma once

#include "KeyCode.h"
#include "Math/Vector.h"  // For vec2
#include "Window/Window.h"  // For derived Input to query focused Window

#include "Log.h"

namespace NXTN {
	class Input
	{
	public:
		static void Init();

		static inline bool IsKeyPressed(KeyCode keycode) {
			return s_Instance->IsKeyPressedImpl(keycode);
		}
		static inline bool IsMouseButtonPressed(MouseButtonCode button) {
			return s_Instance->IsMouseButtonPressedImpl(button);
		}
		static inline vec2 GetMousePos() {
			return s_Instance->GetMousePosImpl();
		}
		static inline vec2 GetMouseMovement() {
			return s_Instance->GetMouseMovementImpl();
		}
		static inline void Update() {
			return s_Instance->UpdateImpl();
		}

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) = 0;
		virtual vec2 GetMousePosImpl() = 0;
		virtual vec2 GetMouseMovementImpl() = 0;
		virtual void UpdateImpl() = 0;

	private:
		static Input* s_Instance;
	};
}
