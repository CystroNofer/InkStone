#pragma once

#include "Event.h"

namespace NXTN {
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {};

		virtual EventFlag GetEventFlag() const override { return EventFlag::Application; }

		virtual EventType GetEventType() const override { return EventType::WindowClosed; }
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height) :m_NewWidth(width), m_NewHeight(height) {};

		virtual EventFlag GetEventFlag() const override { return EventFlag::Application; }

		virtual EventType GetEventType() const override { return EventType::WindowResized; }

		inline int GetNewWidth() const { return m_NewWidth; }

		inline int GetNewHeight() const { return m_NewHeight; }

	private:
		int m_NewWidth, m_NewHeight;
	};

	class ApplicationUpdateEvent : public Event
	{
	public:
		ApplicationUpdateEvent() {};

		virtual EventFlag GetEventFlag() const override { return EventFlag::Application; }

		virtual EventType GetEventType() const override { return EventType::AppUpdate; }
	};

	class ApplicationRenderEvent : public Event
	{
	public:
		ApplicationRenderEvent() {};

		virtual EventFlag GetEventFlag() const override { return EventFlag::Application; }

		virtual EventType GetEventType() const override { return EventType::AppRender; }
	};
}
