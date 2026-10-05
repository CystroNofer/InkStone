#include "pch.h"

#include "TextureManager.h"

#include "Rendering/GraphicsDevice.h"

namespace NXTN {
	HandleMap<Texture2D> TextureManager::s_Map2D;

	Handle<Texture2D> TextureManager::Create(const std::string& filepath)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Failed to create texture at %s: Graphics device not initialized", filepath.c_str());
			return Handle<Texture2D>();
		}

		Texture2D* texPtr = device->CreateTexture2D(filepath);
		if (texPtr == nullptr)
		{
			Log::Warning("Failed to create texture at %s: Texture creation failed", filepath.c_str());
			return Handle<Texture2D>();
		}
		return s_Map2D.Add(texPtr);
	}
	Texture2D* TextureManager::Get(Handle<Texture2D> th)
	{
		return s_Map2D.Get(th);
	}
	bool TextureManager::Validate(Handle<Texture2D> th)
	{
		return s_Map2D.Validate(th);
	}
}
