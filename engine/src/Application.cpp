#include "pch.h"

#include "Application.h"

#include "GLAD/glad.h"
#include "Rendering/GraphicsDevice.h"

namespace NXTN {
	Application::Application() : m_Alive(true), m_Minimized(false)
	{
		NXTN_PROFILE_NEXT(60);
		NXTN_PROFILE_FUNCTION();

		Time::InitTime();

		m_WindowHandle = WindowManager::Create();

		Renderer::Init();

		Input::Init();

		Window* tmpWinPtr = WindowManager::Get(m_WindowHandle);
		unsigned int winWidth = tmpWinPtr->GetWidth();
		unsigned int winHeight = tmpWinPtr->GetHeight();
		UI::Init(winWidth, winHeight);
		// UI config
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
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

		// Camera
		//m_SceneCamera.reset(new Camera(true, 1.0f, m_Window->GetWidth() / (float)m_Window->GetHeight(), 0.01f, 100.0f));
		m_SceneCamera.reset(new Camera(false, 60.0f, winWidth / (float)winHeight, 0.01f, 100.0f));
		m_SceneCameraTransform.reset(new Transform());
		m_SceneCameraTransform->position = vec3(0.0f, 0.0f, -m_CameraDistance);

		m_FrameBuffer.reset(FrameBuffer::Create(winWidth, winHeight));

		//// Temporary draw data
		//// Vertex buffer
		//// 1 3
		//// 0 2
		//float vertices[20] = {
		//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		//	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		//	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		//	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f
		//};

		//// Vertex array layout
		//VertexArrayLayout layout({
		//	{VertexDataType::Float, 3, "Vertex Position"},
		//	{VertexDataType::Float, 2, "Texture Coordinate"}
		//	});

		//// Index buffer
		//unsigned int indices[6] = {
		//	0, 1, 3,
		//	0, 3, 2
		//};

		// Shader, Material, and Texture
		std::expected<Handle<Shader>, std::string> shaderLoadResult = ShaderManager::Load(
			"TestShader",
			{
				{ ShaderStage::Vertex, "../asset/Shader/Texture.vert" },
				{ ShaderStage::Fragment, "../asset/Shader/Texture.frag" }
			}
		);
		if (shaderLoadResult) {
			m_MaterialHandle = MaterialManager::Create(*shaderLoadResult);
		}
		else {
			Log::Error(shaderLoadResult.error().c_str());
		}
		MaterialManager::Get(m_MaterialHandle)->SetUniformTex2D(
			"u_MainTex", TextureManager::Create("../asset/Texture/uv_map_rg_bottom_left_1024.png")
		);

		// Scene
		m_Scene.reset(new Scene());
		//Entity ett = m_Scene->NewEntity<Transform>("Test Entity");
		//m_Scene->AddComponent<Renderable>(
		//	ett,
		//	new Mesh(
		//		VertexArray::Create(VertexBuffer::Create(vertices, 20), layout),
		//		IndexBuffer::Create(indices, 6)
		//	),
		//	m_MaterialHandle
		//);
		std::expected<ModelData, ModelError> importResult = ModelLoader::Import("../asset/Model/CornellBox/CornellBox-Original.obj");
		if (importResult) {
			std::expected<Model, ModelError> uploadResult = ModelLoader::Upload(*importResult);
			if (uploadResult) {
				m_Scene->InstantiateModel(*uploadResult, m_MaterialHandle);
			}
			else
			{
				Log::Warning("%s", uploadResult.error().message.c_str());
			}
		}
		else
		{
			Log::Warning("%s", importResult.error().message.c_str());
		}
	}

	Application::~Application()
	{
		m_Scene.reset();
		m_FrameBuffer.reset();
		UI::Shutdown();
		MaterialManager::Shutdown();
		TextureManager::Shutdown();
		ShaderManager::Shutdown();
		Renderer::Shutdown();
		GraphicsDevice::Shutdown();
		WindowManager::Shutdown();
	}

	void Application::Update()
	{
		//NXTN_PROFILE_FUNCTION()

		Input::Update();

		// ====================== Event ======================
		EventBuffer::PushEvent(new ApplicationUpdateEvent());

		for (Event*& event_ptr : EventBuffer::GetEventBuffer())
		{
			switch (event_ptr->GetEventType())
			{
			case EventType::WindowClosed:
			{
				m_Alive = false;
				return;
			}
			case EventType::WindowResized:
			{
				WindowResizeEvent* e = (WindowResizeEvent*)(event_ptr);
				m_Minimized = e->GetNewWidth() < 1 || e->GetNewHeight() < 1;
				break;
			}
			}
		}

		EventBuffer::Clear();

		// ====================== Time ======================
		Time::Update();

		// ====================== UI ======================
		UI::NewFrame();

		{
			//m_LayerStack.UIUpdate();
			// Prepare dockspace
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);

			// Note: Do not abort even if Begin() returns false (aka window is collapsed)
			// Since dockspace should remain active, otherwise all active windows docked into it will lose their parent and become undocked.
			// Note: Even though dockspace has no titlebar, ImGui requires a non-empty (not "") title for each window
			ImGui::Begin("Dockspace", NULL, m_DockSpaceFlag);
			{
				ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

				// Scene view
				ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
				ImGui::Begin("Scene View", NULL, m_SubWindowFlag);
				{
					ImVec2 viewportSize = ImGui::GetContentRegionAvail();

					// ImGui sometimes return negative viewport sizes
					if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
					{
						// Resize before rendering so the scene and the UI image use the same size this frame.
						if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y)
						{
							m_ViewportSize = viewportSize;
							m_SceneCamera->ResizeViewport(m_ViewportSize.x / m_ViewportSize.y);
							m_FrameBuffer->Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);  // Frame buffer size
						}

						// ImGui records this image now and samples it later in UI::EndFrame().
						ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachment(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));



						// ====================== Camera ======================
						const ImGuiIO& io = ImGui::GetIO();
						if (io.KeyAlt && ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							if (ImGui::IsItemHovered())
							{
								m_OrbitActive = true;
							}
						}
						else
						{
							m_OrbitActive = false;
						}
						if (io.KeyAlt && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
						{
							if (ImGui::IsItemHovered())
							{
								m_PanActive = true;
							}
						}
						else
						{
							m_PanActive = false;
						}

						if (m_OrbitActive) {
							m_CameraYaw = std::remainder(m_CameraYaw + io.MouseDelta.x * 0.005f, NXTN_2_PI_FLOAT);
							m_CameraPitch = std::clamp(
								m_CameraPitch + io.MouseDelta.y * 0.005f,
								-0.5f * NXTN_PI_FLOAT + NXTN_EPSILON,
								0.5f * NXTN_PI_FLOAT - NXTN_EPSILON
							);
							vec3 f = m_SceneCameraTransform->Forward();
						}
						if (m_PanActive) {
							float worldUnitsPerPixel = m_SceneCamera->isOrthographic
								? 2.0f * m_SceneCamera->size / viewportSize.y
								: 2.0f * m_CameraDistance * std::tan(0.5f * m_SceneCamera->size) / viewportSize.y;
							m_CameraTarget += m_SceneCameraTransform->Up() * (io.MouseDelta.y * worldUnitsPerPixel);
							m_CameraTarget += m_SceneCameraTransform->Left() * (io.MouseDelta.x * worldUnitsPerPixel);
							vec3 f = m_SceneCameraTransform->Forward();
						}
						if (io.MouseWheel != 0.0f) {
							m_CameraDistance = std::max(0.1f, m_CameraDistance * std::pow(0.95f, io.MouseWheel));
						}
						m_SceneCameraTransform->rotation = Rotate(
							Rotate(
								Quaternion::Identity(),
								m_CameraYaw,
								vec3{ 0.0f, 1.0f, 0.0f }
							),
							m_CameraPitch,
							vec3{ 1.0f, 0.0f, 0.0f }
						);
						m_SceneCameraTransform->position =
							m_CameraTarget +
							m_SceneCameraTransform->Backward() * m_CameraDistance;
					}
				}
				ImGui::End();

				// Scene Info
				ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
				ImGui::Begin("Scene Info", NULL, m_SubWindowFlag);
				{
					// FPS
					float deltaTime = Time::GetDeltaTime();
					ImGui::Text("FPS: %.0f  (Avg %.2fms/frame)", 1.0f / deltaTime, deltaTime * 1000.0f);

					ImGui::Separator();
				}
				ImGui::End();

				ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
				ImGui::Begin("Hierarchy", nullptr, m_SubWindowFlag);
				{
					if (m_SelectedEntity && !m_Scene->IsAlive(*m_SelectedEntity)) { m_SelectedEntity.reset(); }
					m_SelectedEntity = UI::HierarchyTree(m_Scene->GetHierarchyView(), m_SelectedEntity);
				}
				ImGui::End();

				ImGui::SetNextWindowDockID(m_DockspaceID, ImGuiCond_FirstUseEver);
				ImGui::Begin("Inspector", nullptr, m_SubWindowFlag);
				{
					if (!m_SelectedEntity) {
						ImGui::TextUnformatted("Select an entity in the hierarchy");
					}
					else {
						std::string_view name = m_Scene->GetHierarchyView().GetName(*m_SelectedEntity);
						ImGui::Text("%.*s", static_cast<int>(name.size()), name.data());
						Transform* transform = m_Scene->TryGetComponent<Transform>(*m_SelectedEntity);
						if (!transform) { ImGui::TextUnformatted("No Transform"); }
						else {
							Transform draft = *transform;
							if (UI::TransformEditor(draft)) { *transform = draft; }
						}
					}
				}
				ImGui::End();
			}
			ImGui::End();
		}

		// ====================== Rendering ======================
		if (!m_Minimized && m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			m_FrameBuffer->Bind();

			Renderer::SetClearColor(1.0f, 0.0f, 1.0f);
			Renderer::ClearFrameBuffer();

			m_Scene->Render(*m_SceneCamera, *m_SceneCameraTransform);

			m_FrameBuffer->Unbind();
		}

		UI::EndFrame();

		WindowManager::Get(m_WindowHandle)->Update();
	}

	void Application::Run()
	{
		while (m_Alive)
		{
			Update();
		}
	}
}
