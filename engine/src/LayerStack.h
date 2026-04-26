#pragma once

#include "Layer.h"

namespace NXTN {

	class LayerStack
	{
	public:
		LayerStack();
		virtual ~LayerStack();

		void AddLayer(Layer* layer);

		void Update();
		void UIUpdate();

		void OnEvent(Event*& event_ptr);

	private:
		// Clearly C++ is not happy about Layer being abstract
		// So pointer type is required here
		std::vector<Layer*> m_LayerStack;
	};
}