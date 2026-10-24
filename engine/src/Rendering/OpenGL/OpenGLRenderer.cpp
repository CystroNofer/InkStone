#include "pch.h"

#include "OpenGLRenderer.h"

namespace NXTN {
	OpenGLRenderer::OpenGLRenderer()
		: m_VPMatrix(1.0f)
	{
		glEnable(GL_BLEND);
		// Blend function: newColor = inColor * a + bgColor * (1 - a)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
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

	void OpenGLRenderer::DrawMeshImpl(std::vector<RenderItem>& renderItems)
	{
		// TODO: Sort with other criterias
		std::ranges::sort(
			renderItems,
			[](const RenderItem& left, const RenderItem& right)
			{
				return std::tie(left.materialHandle.id, left.materialHandle.gen)
					< std::tie(right.materialHandle.id, right.materialHandle.gen);
			}
		);

		for (const RenderItem& item : renderItems)
		{
			Material* material = MaterialManager::Get(item.materialHandle);
			if (!item.meshRef || !material) { continue; }
			// Material setters update cached values; Bind uploads them for this draw.
			material->SetUniformMat4("u_ModelMatrix", item.mMatrix);
			material->SetUniformMat4("u_VPMatrix", m_VPMatrix);
			material->Bind();
			item.meshRef->Bind();

			glDrawElements(GL_TRIANGLES, item.meshRef->GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, nullptr);

			item.meshRef->Unbind();
		}
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
