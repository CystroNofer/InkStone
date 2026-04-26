#pragma once

#include "Core.h"

#include "Log.h"
#include "APISetting.h"

#include "Mesh.h"
#include "Shader.h"

namespace NXTN {
	class Renderer
	{
	public:
		virtual ~Renderer() {}

		// Static
		static void SetVPMatrix(const mat4& vpMatrix);
		static void DrawMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Shader>& shader, const mat4& mMatrix);

		static void ResizeViewport(int width, int height);
		
		static void SetClearColor(float r, float g, float b);
		static void ClearFrameBuffer();

		static void Init();

	protected:
		// Virtual
		virtual void SetVPMatrixImpl(const mat4& vpMatrix) = 0;
		virtual void DrawMeshImpl(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Shader>& shader, const mat4& mMatrix) = 0;

		virtual void ResizeViewportImpl(int width, int height) = 0;

		virtual void SetClearColorImpl(float r, float g, float b) = 0;
		virtual void ClearFrameBufferImpl() = 0;

		//static inline bool Initialized() { return s_Renderer != nullptr; }

	private:
		static std::unique_ptr<Renderer> s_Renderer;
	};
}