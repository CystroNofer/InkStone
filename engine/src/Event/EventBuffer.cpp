#include "pch.h"

#include "EventBuffer.h"

namespace NXTN {
	std::deque<Event*> EventBuffer::m_EventBuffer;

	void EventBuffer::PushEvent(Event* event)
	{
		if (m_EventBuffer.size() < NXTN_EVENTBUFFER_MAX)
		{
			m_EventBuffer.emplace_back(event);
		}
		else
		{
			Log::Warning("Event buffer overflowed");
		}
	}

	void EventBuffer::Clear()
	{
		//while (!m_EventBuffer.empty())
		//{
		//	if (m_EventBuffer.front()->GetEventType() == EventType::AppUpdate)
		//	{
		//		m_EventBuffer.pop_front();
		//		break;
		//	}
		//	else
		//	{
		//		m_EventBuffer.pop_front();
		//	}
		//}

		m_EventBuffer.clear();
	}
}
