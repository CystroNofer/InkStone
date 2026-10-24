#include "pch.h"

#include "Scene.h"
#include "Log.h"

#include "Gameplay/Component/Camera.h"
#include "Gameplay/Component/Renderable.h"
#include "Gameplay/Component/Transform.h"
#include "Rendering/Renderer.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace NXTN {
	SceneHierarchyView::SceneHierarchyView(const Scene& scene) : m_Scene(&scene) {}

	std::vector<Entity> SceneHierarchyView::GetRoots() const
	{
		return m_Scene->GetRoots();
	}

	std::span<const Entity> SceneHierarchyView::GetChildren(Entity entity) const
	{
		return m_Scene->GetChildren(entity);
	}

	std::string_view SceneHierarchyView::GetName(Entity entity) const
	{
		const NameTag* name = m_Scene->TryGetComponent<NameTag>(entity);
		return name ? std::string_view(name->name) : std::string_view("Unnamed Object");
	}

	const void* SceneHierarchyView::GetIdentity() const
	{
		return m_Scene;
	}

	SceneHierarchyView Scene::GetHierarchyView() const &
	{
		return SceneHierarchyView(*this);
	}

	Scene::Scene(std::string name) : m_Name(std::move(name)) {

	}

	Scene::~Scene() {

	}

	void Scene::Render(Camera& camera, const Transform& cameraTransform)
	{
		UpdateWorldTransforms();
		Renderer::SetVPMatrix(mul(camera.GetPMatrix(), GetVMatrix(cameraTransform)));

		std::vector<RenderItem> renderItems;
		Each<Renderable, Transform>([&](EntityID id, Renderable& renderable, Transform&)
			{
				if (renderable.meshRef)
				{
					renderItems.emplace_back(renderable.meshRef, renderable.materialHandle, m_Hierarchy[id]->worldMatrix);
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
		if (m_IterationDepth != 0) {
			throw std::logic_error("Cannot create entities during Scene::Each; apply structural changes afterward.");
		}
		EntityID i;
		bool recycled = !m_RecycledEntityIDs.empty();
		if (!recycled) {
			if (m_Generations.size() >= std::numeric_limits<EntityID>::max()) {
				throw std::length_error("Scene entity ID limit reached.");
			}
			i = static_cast<EntityID>(m_Generations.size());
			m_Generations.push_back(0);
			try {
				m_Hierarchy.emplace_back(std::in_place);
			}
			catch (...) {
				m_Generations.pop_back();
				throw;
			}
		}
		else
		{
			i = m_RecycledEntityIDs.back();
			m_RecycledEntityIDs.pop_back();
			m_Hierarchy[i].emplace();
		}
		Entity e{ i, m_Generations[i] };

		try {
			AddComponent<NameTag>(e, std::move(name));
		}
		catch (...) {
			for (std::unique_ptr<IComponentStorage>& storage : m_ComponentStorages) {
				if (storage) { storage->Remove(i); }
			}
			m_Hierarchy[i].reset();
			if (recycled) { m_RecycledEntityIDs.push_back(i); }
			else {
				m_Hierarchy.pop_back();
				m_Generations.pop_back();
			}
			throw;
		}
		Log::Info("New entity: ID %u, Gen %u", i, m_Generations[i]);
		return e;
	}

	bool Scene::SetParent(Entity child, std::optional<Entity> parent)
	{
		if (m_IterationDepth != 0 || !IsAlive(child) || (parent && !IsAlive(*parent))) { return false; }
		if (!TryGetComponent<Transform>(child) || (parent && !TryGetComponent<Transform>(*parent))) { return false; }
		// Walking ancestors is enough: attaching below a descendant would create a cycle.
		for (std::optional<Entity> ancestor = parent; ancestor; ancestor = m_Hierarchy[ancestor->GetID()]->parent) {
			if (*ancestor == child) { return false; }
		}
		HierarchyNode& node = *m_Hierarchy[child.GetID()];
		if (node.parent == parent) { return true; }
		// Allocate first, so allocation failure leaves the original relationship intact.
		if (parent) { m_Hierarchy[parent->GetID()]->children.push_back(child); }
		if (node.parent) { std::erase(m_Hierarchy[node.parent->GetID()]->children, child); }
		node.parent = parent;
		return true;
	}

	std::optional<Entity> Scene::GetParent(Entity entity) const
	{
		return IsAlive(entity) ? m_Hierarchy[entity.GetID()]->parent : std::nullopt;
	}

	std::span<const Entity> Scene::GetChildren(Entity entity) const
	{
		if (!IsAlive(entity)) { return {}; }
		return m_Hierarchy[entity.GetID()]->children;
	}

	std::vector<Entity> Scene::GetRoots() const
	{
		std::vector<Entity> roots;
		const ComponentStorage<Transform>* transforms = TryGetStorage<Transform>();
		if (!transforms) { return roots; }
		for (size_t i = 0; i < m_Hierarchy.size(); ++i) {
			if (m_Hierarchy[i] && !m_Hierarchy[i]->parent && transforms->Has(static_cast<EntityID>(i))) {
				roots.push_back(Entity(static_cast<EntityID>(i), m_Generations[i]));
			}
		}
		return roots;
	}

	void Scene::UpdateWorldTransforms()
	{
		std::vector<Entity> pending = GetRoots();
		// Stack traversal visits roots in ascending entity-ID order.
		std::reverse(pending.begin(), pending.end());
		const ComponentStorage<Transform>* transforms = TryGetStorage<Transform>();
		if (!transforms) { return; }
		while (!pending.empty()) {
			Entity entity = pending.back();
			pending.pop_back();
			// Nonspatial entities cannot contribute transforms or act as spatial parents.
			if (!transforms->Has(entity.GetID())) { continue; }
			HierarchyNode& node = *m_Hierarchy[entity.GetID()];
			mat4 localMatrix = transforms->Get(entity.GetID()).GetMMatrix();
			node.worldMatrix = node.parent ? mul(m_Hierarchy[node.parent->GetID()]->worldMatrix, localMatrix) : localMatrix;
			pending.insert(pending.end(), node.children.rbegin(), node.children.rend());
		}
	}

	std::optional<mat4> Scene::GetWorldMatrix(Entity entity) const
	{
		if (!TryGetComponent<Transform>(entity)) { return std::nullopt; }
		return m_Hierarchy[entity.GetID()]->worldMatrix;
	}

	bool Scene::DestroyEntity(Entity entity)
	{
		if (m_IterationDepth != 0 || !IsAlive(entity)) { return false; }
		// Collect before modifying storage. Reversing this order destroys children first.
		std::vector<Entity> subtree{ entity };
		for (size_t i = 0; i < subtree.size(); ++i) {
			const std::vector<Entity>& children = m_Hierarchy[subtree[i].GetID()]->children;
			subtree.insert(subtree.end(), children.begin(), children.end());
		}
		if (m_RecycledEntityIDs.capacity() - m_RecycledEntityIDs.size() < subtree.size()) {
			m_RecycledEntityIDs.reserve(std::max(m_RecycledEntityIDs.size() + subtree.size(), m_RecycledEntityIDs.capacity() * 2));
		}
		std::optional<Entity> parent = m_Hierarchy[entity.GetID()]->parent;
		if (parent) { std::erase(m_Hierarchy[parent->GetID()]->children, entity); }
		for (std::vector<Entity>::reverse_iterator it = subtree.rbegin(); it != subtree.rend(); ++it) {
			for (std::unique_ptr<IComponentStorage>& storage : m_ComponentStorages) {
				if (storage) { storage->Remove(it->GetID()); }
			}
			if (m_MainCamera && *m_MainCamera == *it) { m_MainCamera.reset(); }
			m_Hierarchy[it->GetID()].reset();
			// Retire exhausted slots instead of wrapping and reviving ancient handles.
			if (m_Generations[it->GetID()] != std::numeric_limits<EntityGeneration>::max()) {
				++m_Generations[it->GetID()];
				m_RecycledEntityIDs.push_back(it->GetID());
			}
			Log::Info("Destroyed entity: ID %u, Gen %u", it->GetID(), it->GetGeneration());
		}
		return true;
	}
}
