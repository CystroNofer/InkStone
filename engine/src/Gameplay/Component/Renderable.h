#pragma once

#include "ECS/Component.h"
#include "Rendering/Mesh.h"
#include "Asset/MaterialManager.h"
#include "Math/Math.h"

namespace NXTN {
	class Renderable : public BaseComponent
	{
	public:
		Renderable();
		// TODO: Update to handles
		Renderable(Mesh* m, Handle<Material> s);

		Handle<Material> materialHandle;

		// TODO: Update to handles
		std::shared_ptr<Mesh> meshRef;
	};
}

