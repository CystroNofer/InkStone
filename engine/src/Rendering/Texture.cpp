#include "pch.h"

#include "APISetting.h"

#include "OpenGL/OpenGLTexture.h"

namespace NXTN {
	Texture2D* Texture2D::Create(const std::string& filepath)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return (Texture2D*) new OpenGLTexture2D(filepath);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}
}
