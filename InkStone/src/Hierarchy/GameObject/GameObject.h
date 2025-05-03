#pragma once

#include "Component/MeshRenderer.h"

namespace NXTN {
	typedef unsigned long long ObjectID;

	class GameObject
	{
	public:
		GameObject(std::string name = "GameObject", Mesh* mesh = nullptr, Shader* shader = nullptr);
		virtual ~GameObject() {}

		void Update();

		inline const std::string& GetName() { return m_Name; }

		Transform transform;

	private:
		static ObjectID s_ObjID;

		ObjectID m_ObjID;

		std::string m_Name;
		std::unique_ptr<MeshRenderer> m_MeshRenderer;
	};
}

