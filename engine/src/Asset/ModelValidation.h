#pragma once

#include "Asset/Model.h"
#include "Gameplay/Component/Transform.h"

#include <cmath>
#include <stdexcept>

// Internal checks shared by model loading and scene instantiation.
namespace NXTN::ModelValidation {
	inline void Require(bool condition, const std::string& message)
	{
		if (!condition) { throw std::runtime_error(message); }
	}

	inline bool Finite(const vec3& value)
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
	}

	inline mat4 LocalMatrix(const ModelTRS& local)
	{
		Transform transform;
		transform.position = local.position;
		transform.rotation = local.rotation;
		transform.scale = local.scale;
		return transform.GetMMatrix();
	}

	inline void ValidateTRS(const ModelTRS& local, const std::string& context)
	{
		Require(Finite(local.position) && Finite(local.scale), context + ": nonfinite position or scale");
		double lengthSquared = static_cast<double>(local.rotation.x) * local.rotation.x
			+ static_cast<double>(local.rotation.y) * local.rotation.y
			+ static_cast<double>(local.rotation.z) * local.rotation.z
			+ static_cast<double>(local.rotation.w) * local.rotation.w;
		Require(std::isfinite(lengthSquared) && std::abs(lengthSquared - 1.0) < 1.0e-4,
			context + ": rotation must be a finite unit quaternion");
		mat4 matrix = LocalMatrix(local);
		for (int row = 0; row < 4; ++row) {
			for (int column = 0; column < 4; ++column) {
				Require(std::isfinite(matrix[row][column]), context + ": TRS matrix overflow");
			}
		}
	}

	inline void ValidateNodes(const std::vector<ModelNode>& nodes, size_t meshCount)
	{
		Require(!nodes.empty() && !nodes.front().parent, "Model must have a root node");
		for (size_t i = 0; i < nodes.size(); ++i) {
			std::string context = "node " + std::to_string(i) + " ('" + nodes[i].name + "')";
			Require(i == 0 || (nodes[i].parent && *nodes[i].parent < i),
				context + ": parent must precede its child; exactly one imported root is required");
			ValidateTRS(nodes[i].localTransform, context);
			for (size_t mesh : nodes[i].meshes) {
				Require(mesh < meshCount, context + ": mesh index out of range");
			}
		}
	}

}
