#include "pch.h"

#include "MeshRenderer.h"

namespace NXTN {
	MeshRenderer::MeshRenderer(Mesh* mesh, Shader* shader)
		: m_Mesh(mesh), m_Shader(shader)
	{

	}

	void MeshRenderer::Update(Transform& transform)
	{
		Renderer::DrawMesh(m_Mesh, m_Shader, transform.GetModelMatrix());
	}
}
