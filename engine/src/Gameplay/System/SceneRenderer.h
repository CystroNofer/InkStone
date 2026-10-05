#pragma once

#include "ECS/ECSSystem.h"
#include "Gameplay/Component/Transform.h"
#include "Gameplay/Component/Renderable.h"
#include "Gameplay/Component/Camera.h"
#include "Rendering/Renderer.h"

namespace NXTN {
	class SceneRenderer : public BaseECSSystem
	{
	public:
		virtual void Run(Registry& registry) override;

		// Placeholder
		void Run(std::unique_ptr<Registry>& registry, std::shared_ptr<Transform> ct, std::shared_ptr<Camera> c);

	private:
		mat4 GetVMatrix(std::shared_ptr<Transform> camT);
	};
}
