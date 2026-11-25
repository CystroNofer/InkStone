#include "pch.h"
#include "ModelLoader.h"

#include "Asset/ModelValidation.h"
#include "Rendering/GraphicsDevice.h"
#include "Rendering/Mesh.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cmath>
#include <format>
#include <stdexcept>
#include <unordered_set>

namespace NXTN {
	namespace {
		using ModelValidation::Finite;
		using ModelValidation::LocalMatrix;
		using ModelValidation::Require;
		using ModelValidation::ValidateNodes;
		using ModelValidation::ValidateTRS;
		constexpr size_t FloatsPerVertex = 8; // Position, UV0, normal (locations 0, 1, 2).
		// Conservatively keep individual buffers below 4 GiB with the current count-based API.
		constexpr size_t MaxFloatCount = std::numeric_limits<unsigned int>::max() / sizeof(float);
		constexpr size_t MaxIndexCount = std::min<size_t>(
			std::numeric_limits<unsigned int>::max() / sizeof(unsigned int), std::numeric_limits<int>::max());

		std::string CopyString(const aiString& value)
		{
			Require(value.length < sizeof(value.data), "Invalid Assimp string length");
			return std::string(value.data, value.length);
		}

		void Warn(ModelData& data, const std::string& context, std::string message)
		{
			Log::Warning("Model '%s', %s: %s", data.source.c_str(), context.c_str(), message.c_str());
			data.warnings.push_back({ context, std::move(message) });
		}

		ModelTRS ImportTRS(const aiMatrix4x4& matrix, ModelData& data,
			const std::string& context, float tolerance)
		{
			for (int row = 0; row < 4; ++row) {
				for (int column = 0; column < 4; ++column) {
					Require(std::isfinite(matrix[row][column]), context + ": nonfinite node matrix");
				}
			}
			Require(std::abs(matrix.d1) <= tolerance && std::abs(matrix.d2) <= tolerance
				&& std::abs(matrix.d3) <= tolerance && std::abs(matrix.d4 - 1.0f) <= tolerance,
				context + ": non-affine node matrix cannot be imported as TRS");

			aiVector3D scale;
			aiQuaternion rotation;
			aiVector3D position;
			aiDecomposeMatrix(&matrix, &scale, &rotation, &position);
			double length = std::sqrt(static_cast<double>(rotation.x) * rotation.x
				+ static_cast<double>(rotation.y) * rotation.y + static_cast<double>(rotation.z) * rotation.z
				+ static_cast<double>(rotation.w) * rotation.w);
			Require(std::isfinite(length) && length > 1.0e-12, context + ": decomposition returned an invalid quaternion");
			ModelTRS result;
			result.position = vec3(position.x, position.y, position.z);
			result.scale = vec3(scale.x, scale.y, scale.z);
			// Assimp and InkStone both expose x/y/z/w; copy explicitly, do not reinterpret their storage
			result.rotation = Quaternion(static_cast<float>(rotation.x / length), static_cast<float>(rotation.y / length),
				static_cast<float>(rotation.z / length), static_cast<float>(rotation.w / length));
			ValidateTRS(result, context);

			mat4 reconstructed = LocalMatrix(result);
			double largestEntry = 0.0;
			double largestDifference = 0.0;
			// Compare the linear block separately: a large translation must not hide lost shear.
			for (int row = 0; row < 3; ++row) {
				for (int column = 0; column < 3; ++column) {
					largestEntry = std::max(largestEntry, std::abs(static_cast<double>(matrix[row][column])));
					largestDifference = std::max(largestDifference,
						std::abs(static_cast<double>(matrix[row][column]) - reconstructed[row][column]));
				}
			}
			double relativeError = largestDifference / std::max(largestEntry, 1.0e-30);
			if (relativeError > tolerance) {
				Warn(data, context, std::format("Lossy TRS conversion (relative linear error {:.6g}); "
					"shear/non-TRS information was discarded. Descendant placement may change", relativeError));
			}
			if (std::abs(length - 1.0) > tolerance) {
				Warn(data, context, "Assimp's rotation quaternion required normalization; source may not be pure TRS");
			}
			double largestScale = std::max({ std::abs(static_cast<double>(scale.x)),
				std::abs(static_cast<double>(scale.y)), std::abs(static_cast<double>(scale.z)) });
			if (std::min({ std::abs(static_cast<double>(scale.x)), std::abs(static_cast<double>(scale.y)),
				std::abs(static_cast<double>(scale.z)) }) <= std::max(largestScale, 1.0) * 1.0e-8) {
				Warn(data, context, "Zero/near-zero scale: rotation may be ambiguous and inverse normal transforms unstable");
			}
			if (static_cast<double>(scale.x) * scale.y * scale.z < 0.0) {
				Warn(data, context, "Reflected transform: preserved signed scale. Face culling must account for world-transform parity");
			}
			return result;
		}

		void ValidateMesh(const MeshData& mesh)
		{
			std::string context = "mesh '" + mesh.name + "'";
			Require(!mesh.vertices.empty() && mesh.vertices.size() <= MaxFloatCount / FloatsPerVertex,
				context + ": empty or oversized vertex buffer");
			Require(!mesh.indices.empty() && mesh.indices.size() % 3 == 0 && mesh.indices.size() <= MaxIndexCount,
				context + ": empty, oversized, or non-triangle index buffer");
			for (unsigned int index : mesh.indices) {
				Require(index < mesh.vertices.size(), context + ": vertex index out of range");
			}
			for (const ModelVertex& vertex : mesh.vertices) {
				Require(Finite(vertex.position) && Finite(vertex.normal)
					&& std::isfinite(vertex.texCoord.x) && std::isfinite(vertex.texCoord.y),
					context + ": nonfinite vertex attribute");
				double normalLengthSquared = static_cast<double>(vertex.normal.x) * vertex.normal.x
					+ static_cast<double>(vertex.normal.y) * vertex.normal.y + static_cast<double>(vertex.normal.z) * vertex.normal.z;
				Require(std::abs(normalLengthSquared - 1.0) < 1.0e-4, context + ": normal must have unit length");
			}
		}
	}

	std::expected<ModelData, ModelError> ModelLoader::Import(const std::string& path, const ModelImportSetting& settings)
	{
		try {
			Require(!path.empty() && path.find('\0') == std::string::npos, "Invalid model filename");
			Require(
				std::isfinite(settings.trsTolerance) && settings.trsTolerance > 0 && settings.trsTolerance < 1,
				"TRS tolerance must be finite and between zero and one"
			);
			Require(
				(aiGetCompileFlags() & ASSIMP_CFLAGS_DOUBLE_SUPPORT) == 0,
				"Assimp DLL precision does not match the single-precision headers"
			);
			unsigned int flags = aiProcess_Triangulate | aiProcess_ValidateDataStructure;
			if (settings.flipUVs) { flags |= aiProcess_FlipUVs; }
			std::unique_ptr<const aiScene, void (*)(const aiScene*)> imported(aiImportFile(path.c_str(), flags), aiReleaseImport);
			Require(imported != nullptr, std::string("Assimp: ") + aiGetErrorString());
			Require(imported->mRootNode && imported->mNumMeshes && imported->mMeshes, "No model geometry/root node");
			ModelData data;
			data.source = path;
			bool generateNormals = false;
			for (unsigned int i = 0; i < imported->mNumMeshes; ++i) {
				Require(imported->mMeshes[i] != nullptr, "Null imported mesh");
				if (!imported->mMeshes[i]->mNormals) {
					generateNormals = true;
					Warn(data, "mesh " + std::to_string(i), "Missing normals; generating smooth normals; hard-edge intent may differ");
				}
			}
			// Generate normals before joining vertices: normal generation needs Assimp's verbose topology
			unsigned int finishingFlags = aiProcess_JoinIdenticalVertices;
			if (generateNormals) { finishingFlags |= aiProcess_GenSmoothNormals; }
			// Post-processing owns/releases its input on failure; take ownership of the returned scene
			imported.reset(aiApplyPostProcessing(imported.release(), finishingFlags));
			Require(imported != nullptr, std::string("Normal/vertex post-processing failed: ") + aiGetErrorString());
			if (imported->mNumAnimations) { Warn(data, "scene", "Animation channels ignored; importing static node transforms only"); }
			if (imported->mNumCameras || imported->mNumLights) { Warn(data, "scene", "Imported cameras and lights ignored"); }
			if (imported->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING) { Warn(data, "scene", "Assimp reported a data-structure validation warning"); }
			if (imported->mMetaData) {
				for (unsigned int i = 0; i < imported->mMetaData->mNumProperties; i++) {
					std::string key = CopyString(imported->mMetaData->mKeys[i]);
					if (key == "UnitScaleFactor" || key == "UpAxis" || key == "CoordAxis") {
						Warn(data, "scene", "Axis/unit metadata present; keeping Assimp's output coordinates without extra engine conversion. Check orientation and size");
						break;
					}
				}
			}

			data.meshes.reserve(imported->mNumMeshes);
			for (unsigned int i = 0; i < imported->mNumMeshes; i++) {
				const aiMesh& mesh = *imported->mMeshes[i];
				std::string context = "mesh " + std::to_string(i) + " ('" + CopyString(mesh.mName) + "')";
				Require(!mesh.mNumBones && !mesh.mNumAnimMeshes, context + ": skinning/morph targets are not supported");
				Require(
					mesh.mVertices && mesh.mNormals && mesh.mFaces && mesh.mNumVertices && mesh.mNumFaces,
					context + ": positions, normals, or faces missing"
				);
				Require(
					mesh.mNumVertices <= MaxFloatCount / FloatsPerVertex && mesh.mNumFaces <= MaxIndexCount / 3,
					context + ": geometry exceeds current GPU count limits"
				);
				MeshData result;
				result.name = CopyString(mesh.mName);
				if (!mesh.mTextureCoords[0]) { Warn(data, context, "UV0 missing; using (0, 0). Texturing will be limited"); }
				if (mesh.mTextureCoords[0] && mesh.mNumUVComponents[0] != 2) {
					Warn(data, context, "UV0 is not two-dimensional; converting to two components");
				}
				bool discardedAttributes = mesh.mTangents || mesh.mBitangents;
				for (unsigned int channel = 0; channel < AI_MAX_NUMBER_OF_COLOR_SETS; channel++) { discardedAttributes |= mesh.mColors[channel] != nullptr; }
				for (unsigned int channel = 1; channel < AI_MAX_NUMBER_OF_TEXTURECOORDS; channel++) { discardedAttributes |= mesh.mTextureCoords[channel] != nullptr; }
				if (discardedAttributes) { Warn(data, context, "Vertex colors, (bi)tangents, or extra UV channels discarded; only position/UV0/normal are retained"); }
				result.vertices.reserve(mesh.mNumVertices);
				for (unsigned int vertex = 0; vertex < mesh.mNumVertices; vertex++) {
					ModelVertex converted;
					converted.position = vec3(mesh.mVertices[vertex].x, mesh.mVertices[vertex].y, mesh.mVertices[vertex].z);
					if (mesh.mTextureCoords[0]) {
						converted.texCoord = vec2(
							mesh.mTextureCoords[0][vertex].x,
							mesh.mNumUVComponents[0] > 1 ? mesh.mTextureCoords[0][vertex].y : 0.0f
						);
					}
					const aiVector3D& normal = mesh.mNormals[vertex];
					double length =
						std::sqrt(static_cast<double>(normal.x) * normal.x + static_cast<double>(normal.y) * normal.y + static_cast<double>(normal.z) * normal.z);
					Require(std::isfinite(length) && length > 0, context + ": unusable normal (possibly degenerate geometry)");
					converted.normal = vec3(static_cast<float>(normal.x / length), static_cast<float>(normal.y / length), static_cast<float>(normal.z / length));
					result.vertices.push_back(converted);
				}
				result.indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3);
				for (unsigned int face = 0; face < mesh.mNumFaces; ++face) {
					Require(mesh.mFaces[face].mNumIndices == 3 && mesh.mFaces[face].mIndices,
						context + ": non-triangle primitive remains; points and lines are unsupported");
					result.indices.insert(result.indices.end(), mesh.mFaces[face].mIndices, mesh.mFaces[face].mIndices + 3);
				}
				ValidateMesh(result);
				data.meshes.push_back(std::move(result));
			}

			struct PendingNode { const aiNode* node; std::optional<size_t> parent; };
			std::vector<PendingNode> pending{ { imported->mRootNode, std::nullopt } };
			std::unordered_set<const aiNode*> visited;
			while (!pending.empty()) {
				PendingNode current = pending.back();
				pending.pop_back();
				Require(current.node && visited.insert(current.node).second, "Invalid or cyclic node hierarchy");
				ModelNode node;
				node.name = CopyString(current.node->mName);
				node.parent = current.parent;
				std::string context = "node " + std::to_string(data.nodes.size()) + " ('" + node.name + "')";
				node.localTransform = ImportTRS(current.node->mTransformation, data, context, settings.trsTolerance);
				Require(!current.node->mNumMeshes || current.node->mMeshes, context + ": missing mesh references");
				for (unsigned int mesh = 0; mesh < current.node->mNumMeshes; ++mesh) {
					Require(current.node->mMeshes[mesh] < data.meshes.size(), context + ": invalid mesh reference");
					node.meshes.push_back(current.node->mMeshes[mesh]);
				}
				size_t nodeIndex = data.nodes.size();
				data.nodes.push_back(std::move(node));
				Require(!current.node->mNumChildren || current.node->mChildren, context + ": missing child references");
				for (unsigned int child = current.node->mNumChildren; child > 0; --child) {
					pending.push_back({ current.node->mChildren[child - 1], nodeIndex });
				}
			}
			ValidateNodes(data.nodes, data.meshes.size());
			return data;
		}
		catch (const std::exception& error) { return std::unexpected(ModelError{ path, "import", error.what() }); }
	}

	std::expected<Model, ModelError> ModelLoader::Upload(const ModelData& data)
	{
		try {
			Require(!data.meshes.empty(), "Model has no meshes");
			ValidateNodes(data.nodes, data.meshes.size());
			for (const MeshData& mesh : data.meshes) { ValidateMesh(mesh); }
			GraphicsDevice* device = GraphicsDevice::Get();
			Require(device != nullptr, "Graphics device is not initialized");
			Model model;
			model.source = data.source;
			model.nodes = data.nodes;
			model.warnings = data.warnings;
			model.meshes.reserve(data.meshes.size());
			const VertexArrayLayout layout{ { VertexDataType::Float, 3, "a_PositionOS" },
				{ VertexDataType::Float, 2, "a_TexCoord" }, { VertexDataType::Float, 3, "a_NormalOS" } };
			for (const MeshData& mesh : data.meshes) {
				// Pack explicitly; engine vector types need not be tightly packed or trivially copyable.
				std::vector<float> vertices;
				vertices.reserve(mesh.vertices.size() * FloatsPerVertex);
				for (const ModelVertex& vertex : mesh.vertices) {
					vertices.insert(vertices.end(), { vertex.position.x, vertex.position.y, vertex.position.z,
						vertex.texCoord.x, vertex.texCoord.y, vertex.normal.x, vertex.normal.y, vertex.normal.z });
				}
				std::shared_ptr<VertexBuffer> vertexBuffer(device->CreateVertexBuffer(vertices.data(), static_cast<unsigned int>(vertices.size())));
				Require(vertexBuffer != nullptr, "Vertex buffer creation failed for mesh '" + mesh.name + "'");
				std::unique_ptr<VertexArray> vertexArray(device->CreateVertexArray(vertexBuffer, layout));
				Require(vertexArray != nullptr, "Vertex array creation failed for mesh '" + mesh.name + "'");
				vertexArray->Bind();
				std::unique_ptr<IndexBuffer> indexBuffer(device->CreateIndexBuffer(mesh.indices.data(), static_cast<unsigned int>(mesh.indices.size())));
				vertexArray->Unbind();
				Require(indexBuffer != nullptr, "Index buffer creation failed for mesh '" + mesh.name + "'");
				// new allocates before releasing either owner; Mesh's constructor only adopts them.
				model.meshes.push_back(std::shared_ptr<Mesh>(new Mesh(vertexArray.release(), indexBuffer.release())));
			}
			return model;
		}
		catch (const std::exception& error) { return std::unexpected(ModelError{ data.source, "upload", error.what() }); }
	}

}
