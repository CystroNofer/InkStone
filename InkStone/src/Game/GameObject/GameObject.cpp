#include "pch.h"

#include "GameObject.h"

namespace NXTN {
	ObjectID GameObject::s_ObjID = 0;

	GameObject::GameObject(std::string name) : m_ObjID(s_ObjID++), m_Name(name) {}

	GameObject::GameObject(const GameObject& other) : m_ObjID(s_ObjID++) {}

	void GameObject::AddComponent(Component* component_ptr)
	{
		m_Components.emplace_back(std::unique_ptr<Component>(component_ptr));
	}

	void GameObject::AddChild(GameObject* child)
	{
		m_Children.emplace_back(std::unique_ptr<GameObject>(child));
	}

	void GameObject::Update()
	{
		for (std::unique_ptr<Component>& c : m_Components)
		{
			c->Update(transform);
		}

		for (std::unique_ptr<GameObject>& c : m_Children)
		{
			c->Update();
		}
	}
}