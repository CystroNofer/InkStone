#pragma once

#include "Event.h"
#include "Input/KeyCode.h"

namespace NXTN {
	class KeyEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() { return m_KeyCode; }

		virtual EventFlag GetEventFlag() const override { return EventFlag::Keyboard; }

	protected:
		KeyEvent(KeyCode keyCode) : m_KeyCode(keyCode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(KeyCode keyCode, unsigned int repeatCount) : KeyEvent(keyCode), m_RepeatCount(repeatCount) {};

		virtual EventType GetEventType() const override { return EventType::KeyPressed; }

		inline unsigned int GetRepeatCount() const { return m_RepeatCount; }

	private:
		unsigned int m_RepeatCount;
	};

	class KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(KeyCode keyCode) : KeyEvent(keyCode) {};

		virtual EventType GetEventType() const override { return EventType::KeyReleased; }
	};
}
