#include "pch.h"

#include "Texture.h"
#include "GraphicsDevice.h"

namespace NXTN {
	Texture2D* Texture2D::Create(const std::string& filepath)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return nullptr;
		}
		return device->CreateTexture2D(filepath);
	}
}
