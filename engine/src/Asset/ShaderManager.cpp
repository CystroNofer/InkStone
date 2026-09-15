#include "pch.h"

#include "Rendering/GraphicsDevice.h"
#include "ShaderManager.h"

namespace NXTN {
	HandleMap<Shader> ShaderManager::s_Map;

	Handle<Shader> ShaderManager::Load(const ShaderProgramDescriptor& description) {
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return Handle<Shader>();
		}

		Shader* p = device->CreateShader(description);

		if (p) {
			return s_Map.Add(p);
		}

		return Handle<Shader>();
	}

	Shader* ShaderManager::Get(Handle<Shader> sh) {
		return s_Map.Get(sh);
	}

	bool ShaderManager::Validate(Handle<Shader> sh) {
		return s_Map.Validate(sh);
	}
}
