#pragma once

#include "Container/HandleMap.h"
#include "Log.h"
#include "Rendering/Shader.h"

namespace NXTN {
	// No need for polymorphism
	class ShaderManager {
	public:
		~ShaderManager() {}

		static Handle<Shader> Load(const std::string& path);
		static Shader* Get(Handle<Shader> sh);
		static bool Validate(Handle<Shader> sh);

	private:
		ShaderManager() = default;

		static HandleMap<Shader> s_Map;
	};
}
