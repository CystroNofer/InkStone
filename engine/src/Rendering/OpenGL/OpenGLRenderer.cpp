#include "pch.h"

#include "OpenGLRenderer.h"

namespace NXTN {
	OpenGLRenderer::OpenGLRenderer()
		: m_VPMatrix(1.0f)
	{
		glEnable(GL_BLEND);
		// Blend function: newColor = inColor * a + bgColor * (1 - a)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRenderer::SetVPMatrixImpl(const mat4& vpMatrix)
	{
		// NDC correction from [0, 1] to [-1, 1]
		mat4 ndcCorrection(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f, -1.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		m_VPMatrix = mul(ndcCorrection, vpMatrix);
	}

	void OpenGLRenderer::DrawMeshImpl(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Shader>& shader, const mat4& mMatrix)
	{
		mesh->Bind();
		shader->Bind();

		shader->SetUniformMat4("u_ModelMatrix", mMatrix);
		shader->SetUniformMat4("u_VPMatrix", m_VPMatrix);

		glDrawElements(GL_TRIANGLES, mesh->GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, nullptr);

		mesh->Unbind();
		shader->Unbind();
	}

	void OpenGLRenderer::ResizeViewportImpl(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void OpenGLRenderer::SetClearColorImpl(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
	}

	void OpenGLRenderer::ClearFrameBufferImpl()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
}
