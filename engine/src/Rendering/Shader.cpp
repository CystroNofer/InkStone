#include "pch.h"

#include "APISetting.h"

#include "OpenGL/OpenGLShader.h"

namespace NXTN {
	Shader* Shader::Create(const std::string& filepath)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return (Shader*) new OpenGLShader(filepath);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}
}
