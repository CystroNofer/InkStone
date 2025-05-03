#pragma once

#include "GameObject/GameObject.h"

namespace NXTN {
	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();
		void AddObject(GameObject* obj);

		const std::vector<std::unique_ptr<GameObject>>& AllObjects();

	private:
		std::vector<std::unique_ptr<GameObject>> m_Objects;
	};
}
