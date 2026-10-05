#pragma once

#include "Core.h"

#include "Log.h"
#include "APISetting.h"

#include "Mesh.h"
#include "Asset/MaterialManager.h"

namespace NXTN {
	struct RenderItem {
		std::shared_ptr<Mesh> meshRef;
		Handle<Material> materialHandle;
		mat4 mMatrix;
	};

	class Renderer
	{
	public:
		virtual ~Renderer() {}

		// Static
		static void SetVPMatrix(const mat4& vpMatrix);
		static void DrawMesh(std::vector<RenderItem>& renderItems);

		static void ResizeViewport(int width, int height);

		static void SetClearColor(float r, float g, float b);
		static void ClearFrameBuffer();

		static void Init();

	protected:
		// Virtual
		virtual void SetVPMatrixImpl(const mat4& vpMatrix) = 0;
		virtual void DrawMeshImpl(std::vector<RenderItem>& renderItems) = 0;

		virtual void ResizeViewportImpl(int width, int height) = 0;

		virtual void SetClearColorImpl(float r, float g, float b) = 0;
		virtual void ClearFrameBufferImpl() = 0;

		//static inline bool Initialized() { return s_Renderer != nullptr; }

	private:
		static std::unique_ptr<Renderer> s_Renderer;
	};
}