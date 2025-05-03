#pragma once

#include "src/Core.h"

#include "src/Layer.h"
#include "src/LayerStack.h"
#include "src/System.h"
#include "src/Time.h"

#include "src/Event/Event.h"
#include "src/Event/ApplicationEvent.h"
#include "src/Event/KeyEvent.h"
#include "src/Event/MouseEvent.h"

#include "src/Hierarchy/GameObject/Component/ObjectComponent.h"
#include "src/Hierarchy/GameObject/Component/MeshRenderer.h"
#include "src/Hierarchy/GameObject/Component/Transform.h"

#include "src/Hierarchy/GameObject/Camera.h"
#include "src/Hierarchy/GameObject/GameObject.h"

#include "src/Hierarchy/Scene.h"

#include "src/Math/Math.h"

#include "src/Rendering/FrameBuffer.h"
#include "src/Rendering/Mesh.h"
#include "src/Rendering/Renderer.h"
#include "src/Rendering/RenderingDataBuffer.h"
#include "src/Rendering/Shader.h"
#include "src/Rendering/Texture.h"
#include "src/Rendering/VertexArray.h"

#include "src/UI/UI.h"

#include "src/Window/Window.h"

#include "src/Application.h"

#ifdef NXTN_PLATFORM_WINDOWS
//extern NXTN::Application* NXTN::CreateApplication();

int main(int argc, char** argv)
{
	//char cwd[MAX_PATH];
	//if (GetCurrentDirectoryA(MAX_PATH, cwd))
	//{
	//	std::cout << "Current Working Directory: " << cwd << std::endl;
	//}

	NXTN::Application* app = NXTN::CreateApplication();
	app->Run();
	delete app;
}
#endif // NXTN_PLATFORM_WINDOWS