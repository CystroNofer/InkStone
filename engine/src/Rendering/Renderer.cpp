#include "pch.h"

#include "Renderer.h"
#include "GraphicsDevice.h"

namespace NXTN {
	std::unique_ptr<Renderer> Renderer::s_Renderer = nullptr;

	void Renderer::Init()
	{
		if (!GraphicsDevice::Init())
		{
			Log::Warning("Renderer initialization failed");
			return;
		}

		s_Renderer.reset(GraphicsDevice::Get()->CreateRenderer());
	}

	void Renderer::SetVPMatrix(const mat4& vpMatrix)
	{
		if (!s_Renderer)
		{
			Log::Warning("Renderer not initialized");
			return;
		}
		s_Renderer->SetVPMatrixImpl(vpMatrix);
	}

	void Renderer::DrawMesh(
		const std::shared_ptr<Mesh>& mesh,
		Shader* const shader,
		const mat4& mMatrix
	) {
		if (!s_Renderer)
		{
			Log::Warning("Renderer not initialized");
			return;
		}
		s_Renderer->DrawMeshImpl(mesh, shader, mMatrix);
	}

	void Renderer::ResizeViewport(int width, int height)
	{
		if (!s_Renderer)
		{
			Log::Warning("Renderer not initialized");
			return;
		}
		s_Renderer->ResizeViewportImpl(width, height);
	}

	void Renderer::SetClearColor(float r, float g, float b)
	{
		if (!s_Renderer)
		{
			Log::Warning("Renderer not initialized");
			return;
		}
		s_Renderer->SetClearColorImpl(r, g, b);
	}

	void Renderer::ClearFrameBuffer()
	{
		if (!s_Renderer)
		{
			Log::Warning("Renderer not initialized");
			return;
		}
		s_Renderer->ClearFrameBufferImpl();
	}
}
