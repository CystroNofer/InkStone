#pragma once

#include "Container/HandleMap.h"
#include "Log.h"
#include "Rendering/Shader.h"

#include <expected>

namespace NXTN {
	class ShaderManager {
	public:
		~ShaderManager() {}

		static std::expected<Handle<Shader>, std::string> Load(
			// The name is for readability and debugging only
			std::string name,
			const std::vector<ShaderStageDescriptor>& stages
		);
		static Shader* Get(Handle<Shader> sh);
		static bool Validate(Handle<Shader> sh);
		static void Shutdown();

	private:
		ShaderManager() = default;

		static HandleMap<Shader> s_Map;
	};
}
