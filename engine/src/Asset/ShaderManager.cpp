#include "pch.h"

#include "APISetting.h"
#include "Rendering/OpenGL/OpenGLShader.h"
#include "ShaderManager.h"

namespace NXTN {
	HandleMap<Shader> ShaderManager::s_Map;

	Handle<Shader> ShaderManager::Load(const std::string& path) {
		Shader* p = nullptr;
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			p = (Shader*)(new OpenGLShader(path));
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		if (p) {
			return s_Map.Add(p);
		}

		return Handle<Shader>::invalid;
	}

	Shader* ShaderManager::Get(Handle<Shader> sh) {
		return s_Map.Get(sh);
	}

	bool ShaderManager::Validate(Handle<Shader> sh) {
		return s_Map.Validate(sh);
	}
}