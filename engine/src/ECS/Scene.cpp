#include "pch.h"

#include "Scene.h"
#include "Log.h"

#include "Gameplay/Component/Camera.h"
#include "Gameplay/Component/Renderable.h"
#include "Gameplay/Component/Transform.h"
#include "Rendering/Renderer.h"

namespace NXTN {
	Scene::Scene(std::string name) : m_Name(std::move(name)) {

	}

	Scene::~Scene() {

	}

	void Scene::Render(Camera& camera, const Transform& cameraTransform)
	{
		Renderer::SetVPMatrix(mul(camera.GetPMatrix(), GetVMatrix(cameraTransform)));

		std::vector<RenderItem> renderItems;
		Each<Transform, Renderable>([&](EntityID, Transform& transform, Renderable& renderable)
			{
				if (renderable.meshRef)
				{
					renderItems.emplace_back(renderable.meshRef, renderable.materialHandle, transform.GetMMatrix());
				}
			});

		Renderer::DrawMesh(renderItems);
	}

	mat4 Scene::GetVMatrix(const Transform& cameraTransform)
	{
		// Look-at origin placeholder V matrix.
		vec3 x, y, z;
		if (dot(cameraTransform.position, cameraTransform.position) < EPSILON_SQ)
		{
			x = vec3(1.0f, 0.0f, 0.0f);
			y = vec3(0.0f, 1.0f, 0.0f);
			z = vec3(0.0f, 0.0f, 1.0f);
		}
		else
		{
			z = normalize(vec3(-cameraTransform.position.x, -cameraTransform.position.y, -cameraTransform.position.z));
			if (z.x * z.x + z.z * z.z < EPSILON_SQ)
			{
				x = vec3(1.0f, 0.0f, 0.0f);
				y = vec3(0.0f, 0.0f, -z.y);
			}
			else
			{
				x = normalize(vec3(z.z, 0.0f, -z.x));
				y = normalize(cross(z, x));
			}
		}

		return mat4(
			x.x, x.y, x.z, -dot(x, cameraTransform.position),
			y.x, y.y, y.z, -dot(y, cameraTransform.position),
			z.x, z.y, z.z, -dot(z, cameraTransform.position),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Entity Scene::NewEntity(std::string name) {
		EntityID i;
		// No recycled ID
		if (m_RecycledEntityIDs.empty()) {
			i = static_cast<EntityID>(m_Generations.size());
			m_Generations.push_back(0);
		}
		// Reuse ID
		else
		{
			i = m_RecycledEntityIDs.back();
			m_RecycledEntityIDs.pop_back();
			// Generation already incremented when destroyed
		}
		Entity e{ i, m_Generations[i] };

		Log::Info("New entity: ID %d, Gen %d", i, m_Generations[i]);

		AddComponent<NameTag>(e, name);

		return e;
	}

	void Scene::DestroyEntity(Entity& e) {
		if (!IsAlive(e)) {
			Log::Warning("Stale entity %d, gen %d\n", e.id, e.generation);
			return;
		}
		for (std::unique_ptr<IComponentStorage>& storagePtr : m_ComponentStorages) {
			if (storagePtr)
			{
				storagePtr->Remove(e.id);
			}
		}
		if (m_MainCamera && *m_MainCamera == e)
		{
			m_MainCamera.reset();
		}
		m_RecycledEntityIDs.push_back(e.id);
		m_Generations[e.id]++;

		Log::Info("Destroyed entity: ID %d, Gen %d", e.id, e.generation);
	}
}
