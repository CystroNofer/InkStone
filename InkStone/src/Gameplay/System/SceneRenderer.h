#pragma once

#include "src/ECS/ECSSystem.h"
#include "src/Gameplay/Component/Transform.h"
#include "src/Gameplay/Component/Renderable.h"
#include "src/Gameplay/Component/Camera.h"
#include "src/Rendering/Renderer.h"

namespace NXTN {
	class SceneRenderer : public BaseECSSystem
	{
	public:
		virtual void Run(Registry& registry) override;

		// Placeholder
		void Run(std::unique_ptr<Registry>& registry, std::shared_ptr<Transform> ct, std::shared_ptr<Camera> c);

	private:
		struct RenderItem {
			std::shared_ptr<Mesh> meshRef;
			std::shared_ptr<Shader> shaderRef;
			mat4 mMatrix;
		};

		mat4 GetVMatrix(std::shared_ptr<Transform> camT);
	};
}
