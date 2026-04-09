#pragma once

#include "src/ECS/Component.h"
#include "src/Rendering/Mesh.h"
#include "src/Rendering/Shader.h"

namespace NXTN {
	class Renderable : public BaseComponent
	{
	public:
		Renderable();
		// TODO: Update to handles
		Renderable(Mesh* m, Shader* s);

		// TODO: Add materials


		// TODO: Update to handles
		std::shared_ptr<Mesh> meshRef;
		std::shared_ptr<Shader> shaderRef;
	};
}

