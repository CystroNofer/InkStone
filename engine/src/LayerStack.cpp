#include "pch.h"

#include "LayerStack.h"

namespace NXTN {

	LayerStack::LayerStack() {}

	LayerStack::~LayerStack() {}

	void LayerStack::AddLayer(Layer* layer)
	{
		m_LayerStack.emplace_back(std::move(layer));
	}

	void LayerStack::Update()
	{
		NXTN_PROFILE_FUNCTION()

		for (Layer* layer : m_LayerStack)
			layer->Update();
	}

	void LayerStack::UIUpdate()
	{
		NXTN_PROFILE_FUNCTION()

		for (Layer* layer : m_LayerStack)
			layer->UIUpdate();
	}

	void LayerStack::OnEvent(Event*& event_ptr)
	{
		for (Layer* layer : m_LayerStack)
		{
			if (layer->OnEvent(event_ptr))
			{
				return;
			}
		}
	}
}
