#include "pch.h"

#include "GraphicsDevice.h"

#include "APISetting.h"
#include "Log.h"
#include "OpenGL/OpenGLGraphicsDevice.h"

namespace NXTN {
	std::unique_ptr<GraphicsDevice> GraphicsDevice::s_Device = nullptr;

	bool GraphicsDevice::Init()
	{
		if (s_Device)
		{
			return true;
		}

		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL:
			s_Device = std::make_unique<OpenGLGraphicsDevice>();
			return true;
		case GraphicsAPI::None:
			Log::Warning("No graphics API specified");
			return false;
		default:
			Log::Warning("Unsupported graphics API");
			return false;
		}
	}

	GraphicsDevice* GraphicsDevice::Get()
	{
		return s_Device.get();
	}
}
