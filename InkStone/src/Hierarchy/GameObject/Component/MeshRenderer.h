#pragma once

#include "Transform.h"

#include "src/Rendering/Renderer.h"

namespace NXTN {
	class MeshRenderer : public ObjectComponent
	{
	public:
		MeshRenderer(Mesh* mesh, Shader* shader);
		~MeshRenderer() {}

		void Update(const mat4& modelToWorldMatrx);

	private:
		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<Shader> m_Shader;
	};
}
