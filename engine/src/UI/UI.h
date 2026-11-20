#pragma once

#include "ECS/Entity.h"
#include "Gameplay/Component/Transform.h"
#include <optional>

namespace NXTN {
	class SceneHierarchyView;
	class UI
	{
	public:
		virtual ~UI() {}

		static void Init(unsigned int w, unsigned int h);
		static void Shutdown();
		
		// Public static interface/wrapper
		static void NewFrame();
		static void EndFrame();
		// Returns the selected entity; scene mutation remains with the caller.
		static std::optional<Entity> HierarchyTree(SceneHierarchyView hierarchy, std::optional<Entity> selected);
		// Edits a caller-owned copy and reports whether any value changed.
		static bool TransformEditor(Transform& draft);
		

	protected:
		UI() {}

		// Protected virtual implementation
		virtual void NewFrameImpl() = 0;
		virtual void EndFrameImpl() = 0;
		virtual std::optional<Entity> HierarchyTreeImpl(SceneHierarchyView hierarchy, std::optional<Entity> selected) = 0;
		virtual bool TransformEditorImpl(Transform& draft) = 0;


	private:
		static std::unique_ptr<UI> s_UI;
	};
}

