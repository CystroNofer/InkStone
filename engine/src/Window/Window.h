#pragma once

//#include "Core.h"

#include "APISetting.h"
#include "Log.h"
#include "Event/EventBuffer.h"

namespace NXTN {
	class Window
	{
	public:
		//using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void Update() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enabled) = 0;

		//virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(std::string title = "InkStone");
	};
}
