#include "pch.h"

#include "Scene.h"

namespace NXTN {
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
		for (std::unique_ptr<GameObject>& objPtr : m_Objects)
		{
			objPtr.release();
		}
	}

	void Scene::Update()
	{
		for (std::unique_ptr<GameObject>& obj : m_Objects)
		{
			obj->Update();
		}
	}

	void Scene::AddObject(GameObject* obj)
	{
		m_Objects.emplace_back(obj);
	}

	const std::vector<std::unique_ptr<GameObject>>& Scene::AllObjects()
	{
		return m_Objects;
	}
}
