#include "pch.h"
#include "SceneRenderer.h"

namespace NXTN {
	void SceneRenderer::Run(Registry& registry) {

	}

	void SceneRenderer::Run(std::unique_ptr<Registry>& registry, std::shared_ptr<Transform> ct, std::shared_ptr<Camera> c) {
		Renderer::SetVPMatrix(mul(c->GetPMatrix(), GetVMatrix(ct)));

		std::vector<RenderItem> renderItems;

		registry->Each<Transform, Renderable>([&](EntityID _, Transform& t, Renderable& r)
			{
				if (r.meshRef && r.shaderRef) renderItems.emplace_back(r.meshRef, r.shaderRef, t.GetMMatrix());
			}
		);

		for (RenderItem& i : renderItems) {
			Renderer::DrawMesh(i.meshRef, i.shaderRef, i.mMatrix);
		}
	}

	mat4 SceneRenderer::GetVMatrix(std::shared_ptr<Transform> camT) {
		// Look-at origin placeholder V matrix
		vec3 x, y, z;
		
		if (dot(camT->position, camT->position) < 1e-12f) {
			x = vec3(1.0f, 0.0f, 0.0f);
			y = vec3(0.0f, 1.0f, 0.0f);
			z = vec3(0.0f, 0.0f, 1.0f);
		}
		else {
			z = normalize(-camT->position);  // Look direction

			// Z facing up - X aligns with world X, Y aligns with world -Z
			if (z.x * z.x + z.z * z.z < 1e-12f) {
				x = vec3(z.y, 0.0f, 0.0f);
				y = vec3(0.0f, 0.0f, -z.y);
			}
			else {
				x = normalize(vec3(z.z, 0.0f, -z.x));  // Cross((0, 1, 0), z)
				y = normalize(cross(z, x));
			}
		}

		return mat4(
			x.x, x.y, x.z, -dot(x, camT->position),
			y.x, y.y, y.z, -dot(y, camT->position),
			z.x, z.y, z.z, -dot(z, camT->position),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
}