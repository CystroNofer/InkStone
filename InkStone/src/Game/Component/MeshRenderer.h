#pragma once

#include "Component.h"

#include "src/Rendering/Renderer.h"

namespace NXTN {
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(Mesh* mesh, Shader* shader);
		~MeshRenderer() {}

		virtual void Update(Transform& transform) override;

	private:
		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<Shader> m_Shader;
	};
}
