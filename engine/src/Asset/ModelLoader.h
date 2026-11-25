#pragma once

#include "Asset/Model.h"

#include <expected>

namespace NXTN {
	class ModelLoader {
	public:
		// CPU-only, static triangle geometry
		// Imported materials are deliberately ignored
		static std::expected<ModelData, ModelError> Import(
			const std::string& path, const ModelImportSetting& settings = {}
		);
		// Requires a current graphics context on the graphics thread
		// Attribute order: position, UV0, normal
		static std::expected<Model, ModelError> Upload(const ModelData& data);
	};
}
