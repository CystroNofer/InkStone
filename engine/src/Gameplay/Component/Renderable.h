#pragma once

#include "ECS/Component.h"
#include "Rendering/Mesh.h"
#include "Asset/ShaderManager.h"
#include "Math/Math.h"

namespace NXTN {
	class Renderable : public BaseComponent
	{
	public:
		Renderable();
		// TODO: Update to handles
		Renderable(Mesh* m, Handle<Shader> s);

		// TODO: Add materials


		// TODO: Update to handles
		std::shared_ptr<Mesh> meshRef;
		Handle<Shader> shaderHandle;
	};
}

