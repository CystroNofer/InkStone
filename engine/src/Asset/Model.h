#pragma once

#include "Math/Math.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace NXTN {
	class Mesh;

	struct ModelVertex {
		vec3 position = vec3(0.0f);
		vec2 texCoord = vec2(0.0f);
		vec3 normal = vec3(0.0f);
	};

	struct MeshData {
		std::string name;
		std::vector<ModelVertex> vertices;
		std::vector<unsigned int> indices;
	};

	struct ModelTRS {
		vec3 position = vec3(0.0f);
		Quaternion rotation;
		vec3 scale = vec3(1.0f);
	};

	// Hierarchy node within the model
	struct ModelNode {
		std::string name;
		std::optional<size_t> parent;
		ModelTRS localTransform;
		std::vector<size_t> meshes;
	};

	// Import(...) logs these once
	// And retains them on both ModelData and Model for inspectors
	struct ModelWarning {
		std::string context;
		std::string message;
	};

	struct ModelError {
		std::string source;
		std::string stage;
		std::string message;
	};

	struct ModelImportSetting {
		bool flipUVs = false;
		float trsTolerance = 1.0e-4f;
	};

	struct ModelData {
		std::string source;
		std::vector<MeshData> meshes;
		std::vector<ModelNode> nodes;
		std::vector<ModelWarning> warnings;
	};

	struct Model {
		std::string source;
		// Instances share these owners
		// Release all GPU owners before destroying the context
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<ModelNode> nodes;
		std::vector<ModelWarning> warnings;
	};
}
