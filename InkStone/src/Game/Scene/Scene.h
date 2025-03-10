#pragma once

#include "src/Game/GameObject/GameObject.h"

namespace NXTN {
	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		void AddObject(GameObject* gameObj);
		bool DeleteObject(unsigned long long objID);
		inline size_t GetObjectCount() const { return m_GameObjects.size(); }
		inline const std::vector<std::unique_ptr<GameObject>>& AllObjects() const { return m_GameObjects; }

		void Update();

	private:
		std::vector<std::unique_ptr<GameObject>> m_GameObjects;
	};
}
