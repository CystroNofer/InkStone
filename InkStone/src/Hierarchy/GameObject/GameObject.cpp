#include "pch.h"

#include "GameObject.h"

namespace NXTN {
	ObjectID GameObject::s_ObjID = 0;

	GameObject::GameObject(std::string name, Mesh* mesh, Shader* shader) : m_ObjID(s_ObjID++), m_Name(name)
	{
		if (mesh && shader)
		{
			m_MeshRenderer.reset(new MeshRenderer(mesh, shader));
		}
	}

	void GameObject::Update()
	{
		if (m_MeshRenderer) m_MeshRenderer->Update(transform.GetModelMatrix());
	}
}