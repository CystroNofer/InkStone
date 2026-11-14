#pragma once

#include "UI/UI.h"

#include "ImGuiBackend/imgui_impl_glfw.h"
#include "ImGuiBackend/imgui_impl_opengl3.h"

namespace NXTN {
	class OpenGLUI : public UI
	{
	public:
		OpenGLUI(unsigned int w, unsigned int h);
		~OpenGLUI() {}

		virtual void NewFrameImpl() override;
		virtual void EndFrameImpl() override;
		virtual std::optional<Entity> HierarchyTreeImpl(SceneHierarchyView hierarchy, std::optional<Entity> selected) override;
		virtual bool TransformEditorImpl(Transform& draft) override;

		virtual void OnEventImpl(Event*& event_ptr) override;
	};
}
