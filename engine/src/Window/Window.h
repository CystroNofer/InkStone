#pragma once

//#include "Core.h"

#include "APISetting.h"
#include "Log.h"
#include "Event/EventBuffer.h"

#include <cmath>

namespace NXTN {
	// Forward declaration
	class WindowManager;

	class Window
	{
		friend WindowManager;
	public:
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

		virtual void Update() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enabled) = 0;

		virtual void* GetNativeWindow() const = 0;

	protected:
		Window() = default;
		virtual ~Window() {}
	};

	struct WindowHandle {
		size_t id;
		uint32_t gen;
	};

	class WindowManager
	{
	public:
		static WindowHandle Create(std::string title = "InkStone");
		// Returning Window* is safe since
		// The deconstructor is protected (friend to WindowManager)
		// And the copy constructor is deleted
		static Window* Get(WindowHandle& wh);
		static void Destroy(WindowHandle& wh);
		static void OnFocused(Window* winPtr, bool focused);
		static Window* GetFocused();

	private:
		WindowManager() {}
		~WindowManager() {}

		static std::vector<std::pair<uint32_t, Window*>> s_Windows;
		static WindowHandle s_LastFocusedHandle;
	};
}
