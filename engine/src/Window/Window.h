#pragma once

//#include "Core.h"

#include "APISetting.h"
#include "Container/HandleMap.h"
#include "Event/EventBuffer.h"
#include "Log.h"

#include <cmath>

namespace NXTN {
	// Forward declaration
	class WindowManager;

	class Window
	{
	public:
		virtual ~Window() {}

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

		virtual void SetFocusedCallback(std::function<void(bool)> callback) = 0;

	protected:
		Window() = default;
	};

	class WindowManager
	{
	public:
		static Handle<Window> Create(std::string title = "InkStone");
		// Returning Window* is safe since
		// The deconstructor is protected (friend to WindowManager)
		// And the copy constructor is deleted
		static Window* Get(Handle<Window>& wh);
		static void Destroy(Handle<Window>& wh);
		static void OnFocused(Handle<Window> wh, bool focused);
		static Window* GetFocused();

	private:
		WindowManager() {}
		~WindowManager() {}

		static HandleMap<Window> s_Windows;
		static Handle<Window> s_LastFocusedHandle;
	};
}
