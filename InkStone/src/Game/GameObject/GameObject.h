#pragma once

#include "src/Game/Component/Component.h"

namespace NXTN {
	typedef unsigned long long ObjectID;

	class GameObject
	{
	public:
		GameObject(std::string name = "GameObject");
		GameObject(const GameObject& other);
		virtual ~GameObject() {}

		void AddComponent(Component* component_ptr);

		void AddChild(GameObject* child);

		void Update();

		inline ObjectID GetID() const { return m_ObjID; }
		inline const std::string& GetName() const { return m_Name; }
		inline const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return m_Children; }

		Transform transform;

	private:
		std::vector<std::unique_ptr<Component>> m_Components;

		std::vector<std::unique_ptr<GameObject>> m_Children;

	private:
		static ObjectID s_ObjID;

		ObjectID m_ObjID;

		std::string m_Name;
	};
}

