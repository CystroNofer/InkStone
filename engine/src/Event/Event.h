#pragma once

#include "Core.h"

//#include "pch.h"

namespace NXTN {
	enum class EventType
	{
		None = 0,
		AppUpdate, AppRender,
		WindowClosed, WindowResized,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseScroll, MouseMove
	};

	enum class EventFlag
	{
		None = 0,
		Application    = BITMASK(0),
		Keyboard       = BITMASK(1),
		Mouse          = BITMASK(2),
		Input          = BITMASK(1) | BITMASK(2),
		Window         = BITMASK(3)
	};

	class Event
	{
	public:
		virtual EventType GetEventType() const { return EventType::None; }
		virtual EventFlag GetEventFlag() const { return EventFlag::None; }
		//virtual std::string ToString() const {}

	protected:
		bool m_Handled = false;
	};
}