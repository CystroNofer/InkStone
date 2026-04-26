#pragma once

#include "pch.h"

#include "Core.h"

#include "APISetting.h"
#include "EngineTime.h"
#include "Layer.h"
#include "LayerStack.h"
#include "Log.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "ECS/Registry.h"
#include "ECS/Scene.h"

#include "Gameplay/Component/Camera.h"
#include "Gameplay/Component/Renderable.h"
#include "Gameplay/Component/Transform.h"
#include "Gameplay/System/SceneRenderer.h"

#include "Input/Input.h"

#include "Math/Math.h"

#include "Rendering/FrameBuffer.h"
#include "Rendering/Mesh.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingDataBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Rendering/VertexArray.h"

#include "UI/UI.h"

#include "Window/Window.h"

#include <imgui.h>

namespace NXTN {
	class Application
	{
	public:
		Application();
		~Application();

		void Update();

		void Run();

	private:
		std::shared_ptr<Window> m_Window;

		bool m_Alive;
		bool m_Minimized;

	private:
		std::shared_ptr<Camera> m_SceneCamera;
		std::shared_ptr<Transform> m_SceneCameraTransform;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		//std::shared_ptr<Scene> m_TestScene;
		std::shared_ptr<Texture2D> m_Texture;
		Shader* m_Shader;

		std::unique_ptr<Registry> m_Registry;
		std::unique_ptr<SceneRenderer> m_SceneRenderer;

		ImGuiID m_DockspaceID;
		ImGuiWindowFlags m_DockSpaceFlag;
		ImGuiWindowFlags m_SubWindowFlag;

		ImVec2 m_ViewportSize;
	};

	//Application* CreateApplication();
}

#ifdef NXTN_PLATFORM_WINDOWS
//extern NXTN::Application* NXTN::CreateApplication();

int main(int argc, char** argv)
{
	//char cwd[MAX_PATH];
	//if (GetCurrentDirectoryA(MAX_PATH, cwd))
	//{
	//	std::cout << "Current Working Directory: " << cwd << std::endl;
	//}

	//NXTN::Application* app = NXTN::CreateApplication();
	NXTN::Application* app = new NXTN::Application();
	app->Run();
	delete app;
}
#endif // NXTN_PLATFORM_WINDOWS
