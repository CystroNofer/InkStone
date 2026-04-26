#include "pch.h"

#include "APISetting.h"

namespace NXTN {
	GraphicsAPI APISetting::s_GraphicsAPI = GraphicsAPI::OpenGL;
	WindowAPI   APISetting::s_WindowAPI   = WindowAPI::GLFW;
}
