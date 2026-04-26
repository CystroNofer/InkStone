#pragma once

#include "Event/EventBuffer.h"

namespace NXTN {

	class Layer
	{
	public:
		virtual ~Layer() {}

		virtual void Update() = 0;
		virtual void UIUpdate() = 0;
		virtual inline bool OnEvent(Event*& event_ptr) = 0;

		inline const char* GetName() const { return m_Name; }

	protected:
		Layer(const char* name = "Layer") : m_Name(name) {}

		const char* m_Name;
	};
}

