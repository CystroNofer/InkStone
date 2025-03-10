#include "InkStoneEditor.h"

namespace NXTN {
	// Editor Layer
	EditorLayer::EditorLayer(unsigned int windowWidth, unsigned int windowHeight)
	{
		m_Name = "Sandbox Gameplay Layer";

		// Camera
		m_SceneCamera.reset(new Camera(1.0f, vec2i(windowWidth, windowHeight), false));
		m_SceneCamera->transform.SetPosition(0.0f, 0.0f, -10.0f);

		m_FrameBuffer.reset(FrameBuffer::Create(windowWidth, windowHeight));

		// Temporary draw data
		// Vertex buffer
		// 1 3
		// 0 2
		float vertices[20] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f
		};

		// Vertex array layout
		VertexArrayLayout layout({
			{VertexDataType::Float, 3, "Vertex Position"},
			{VertexDataType::Float, 2, "Texture Coordinate"}
			});

		// Index buffer
		unsigned int indices[6] = {
			0, 1, 3,
			0, 3, 2
		};

		// Shader
		m_Shader.reset(Shader::Create("Asset/Shader/Texture.glsl"));
		// Texture
		m_Texture.reset(Texture2D::Create("Asset/Texture/Logo.png"));

		// Scene
		m_TestScene.reset(new Scene);
		GameObject* testObj = new GameObject();
		testObj->AddComponent(
			new MeshRenderer(
				new Mesh(
					VertexArray::Create(VertexBuffer::Create(vertices, 20), layout),
					IndexBuffer::Create(indices, 6)
				),
				m_Shader.get()
			)
		);
		m_TestScene->AddObject(testObj);

		// UI config
		// Create dockspace
		UI::NewFrame();
		m_DockspaceID = ImGui::GetID("MainDockSpace");
		UI::EndFrame();
		// Window flag
		m_DockSpaceFlag =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;
		m_SubWindowFlag =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse;
	}

	void EditorLayer::Update()
	{
		m_FrameBuffer->Bind();

		// Rendering
		Renderer::SetClearColor(1.0f, 0.0f, 1.0f);

		Renderer::ClearFrameBuffer();

		Renderer::SetVPMatrix(m_SceneCamera->GetViewProjMatrix());

		m_Texture->Bind(0);
		m_Shader->SetUniformInt("u_MainTex", 0);

		m_TestScene->Update();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::UIUpdate()
	{
		// Prepare dockspace
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		// Note: Do not abort even if Begin() returns false (aka window is collapsed)
		// Since dockspace should remain active, otherwise all active windows docked into it will lose their parent and become undocked.
		// Note: Even though dockspace has no titlebar, ImGui requires a non-empty (not "") title for each window
		ImGui::Begin("Dockspace", NULL, m_DockSpaceFlag);
		{
			ImGui::PopStyleVar(3);

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render the background and handle pass-thru holes, so Begin() should not render a background.
			ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None /*ImGuiWindowFlags_NoBackground*/);

			// View window
			ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
			ImGui::Begin("View", NULL, m_SubWindowFlag);
			{
				// View
				ImVec2 viewportSize = ImGui::GetContentRegionAvail();
				// ImGui sometimes return negative viewport sizes
				if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
				{
					// Resize if necessary
					// Note: ImGui seems to only bind the color attachment on ImGui::Image call
					// And render AFTER ImGui::Render()
					// Thus, flickering is unavoidable
					if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y)
					{
						m_ViewportSize = viewportSize;
						Renderer::ResizeViewport((int)m_ViewportSize.x, (int)m_ViewportSize.y);  // OpenGL viewport
						m_SceneCamera->ResizeViewport((int)m_ViewportSize.x, (int)m_ViewportSize.y);  // Camera aspect ratio
						m_FrameBuffer->Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);  // Frame buffer size
					}
				}
				// Note:
				// ImGui::Image defines uv0 and uv1 as the top-left and the bottom-right corner
				// While OpenGL defines uv0 and uv1 as the bottom-left and the top-right corner
				// The uv0 and uv1 parameters are manully set to fix this
				ImGui::Image((ImTextureID)(intptr_t)m_FrameBuffer->GetColorAttachment(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			}
			ImGui::End();

			// Scene Info
			ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
			ImGui::Begin("Scene Info", NULL, m_SubWindowFlag);
			{
				// FPS
				float deltaTime = Time::GetDeltaTime();
				ImGui::Text("FPS: %.0f  (Avg %.2fms/frame)", 1000.0f / deltaTime, deltaTime * 1000.0f);

				ImGui::Separator();

				DrawHierarchy(m_TestScene->AllObjects());
			}
			ImGui::End();
		}
		ImGui::End();
	}

	bool EditorLayer::OnEvent(Event*& event_ptr)
	{
		if (event_ptr->GetEventType() == EventType::WindowResized)
		{
			Renderer::ResizeViewport((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);
		}
		return false;
	}

	void EditorLayer::DrawHierarchy(const std::vector<std::unique_ptr<GameObject>>& gameObjects)
	{
		for (const std::unique_ptr<GameObject>& gameObj : gameObjects)
		{
			if (ImGui::TreeNode(gameObj->GetName().c_str()))
			{
				DrawHierarchy(gameObj->GetChildren());
				// TODO: List components and/or other information
				ImGui::TreePop();
			}
		}
	}

	// Sandbox Application
	InkStoneEditor::InkStoneEditor()
		: Application()
	{
		m_LayerStack.AddLayer(new EditorLayer(m_Window->GetWidth(), m_Window->GetHeight()));
	}

	void InkStoneEditor::Update()
	{
		Application::Update();
	}

	Application* CreateApplication()
	{
		return new InkStoneEditor();
	}
}