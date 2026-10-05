#include "pch.h"

#include "MaterialManager.h"

namespace NXTN {
	HandleMap<Material> MaterialManager::s_Map;

	Handle<Material> MaterialManager::Create(Handle<Shader> shaderHandle)
	{
		Material* material = new Material(shaderHandle);
		return s_Map.Add(material);
	}

	Material* MaterialManager::Get(Handle<Material> mh)
	{
		return s_Map.Get(mh);
	}
	bool MaterialManager::Validate(Handle<Material> mh)
	{
		return s_Map.Validate(mh);
	}
}
