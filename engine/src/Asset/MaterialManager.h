#pragma once

#include "Container/HandleMap.h"
#include "Rendering/Material.h"

namespace NXTN {
	class MaterialManager
	{
	public:
		~MaterialManager() {}

		static Handle<Material> Create(Handle<Shader> shaderHandle);
		static Material* Get(Handle<Material> mh);
		static bool Validate(Handle<Material> mh);

	private:
		MaterialManager() = default;

		static HandleMap<Material> s_Map;
	};
}
