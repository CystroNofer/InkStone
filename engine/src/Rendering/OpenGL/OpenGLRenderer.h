#pragma once

#include "Rendering/Renderer.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

namespace NXTN {
	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer();
		virtual ~OpenGLRenderer() {}

		virtual void SetVPMatrixImpl(const mat4& vpMatrix) override;
		virtual void DrawMeshImpl(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Shader>& shader, const mat4& mMatrix) override;

		virtual void ResizeViewportImpl(int width, int height) override;

		virtual void SetClearColorImpl(float r, float g, float b) override;
		virtual void ClearFrameBufferImpl() override;

	private:
		mat4 m_VPMatrix;
	};
}

