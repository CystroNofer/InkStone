#pragma once

#include "Event/Event.h"

namespace NXTN {
	class SceneHierarchyView;
	class UI
	{
	public:
		virtual ~UI() {}

		static void Init(unsigned int w, unsigned int h);
		
		// Public static interface/wrapper
		static void NewFrame();
		static void EndFrame();
		// Draws into the current UI container; no selection or scene mutation.
		static void HierarchyTree(SceneHierarchyView hierarchy);
		
		static void OnEvent(Event*& event_ptr);

	protected:
		UI() {}

		// Protected virtual implementation
		virtual void NewFrameImpl() = 0;
		virtual void EndFrameImpl() = 0;
		virtual void HierarchyTreeImpl(SceneHierarchyView hierarchy) = 0;

		virtual void OnEventImpl(Event*& event_ptr) = 0;

	private:
		static std::unique_ptr<UI> s_UI;
	};
}

