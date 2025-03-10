#include "pch.h"

#include "Scene.h"

namespace NXTN {
	void Scene::AddObject(GameObject* gameObj)
	{
		m_GameObjects.emplace_back(gameObj);
	}

	bool Scene::DeleteObject(unsigned long long objID)
	{
		for (size_t i = 0; i < m_GameObjects.size(); i++)
		{
			if (m_GameObjects[i]->GetID() == objID)
			{
				m_GameObjects[i] = std::move(m_GameObjects.back());
				m_GameObjects.pop_back();

				return true;
			}
		}
		return false;
	}

	void Scene::Update()
	{
		for (std::unique_ptr<GameObject>& gameObj : m_GameObjects)
		{
			gameObj->Update();
		}
	}
}
