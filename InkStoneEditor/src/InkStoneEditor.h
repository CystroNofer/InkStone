#pragma once

#include "InkStone.h"

#include "include/imgui/imgui.h"

#define NXTN_EDITOR

namespace NXTN {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer(unsigned int windowWidth, unsigned int windowHeight);

		virtual void Update() override;
		virtual void UIUpdate() override;

		virtual bool OnEvent(Event*& event_ptr) override;

	private:
		//void DrawEntities(const std::vector<std::unique_ptr<Entity>>& entities);

	private:
		//std::shared_ptr<Camera> m_SceneCamera;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<Scene> m_TestScene;
		//std::shared_ptr<Texture2D> m_Texture;
		//std::shared_ptr<Shader> m_Shader;

		ImGuiID m_DockspaceID;
		ImGuiWindowFlags m_DockSpaceFlag;
		ImGuiWindowFlags m_SubWindowFlag;

		ImVec2 m_ViewportSize;
	};

	class InkStoneEditor : public Application
	{
	public:
		InkStoneEditor();
		~InkStoneEditor() {}

		virtual void Update() override;
	};
}

