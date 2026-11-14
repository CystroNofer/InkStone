#include "pch.h"

#include "Renderable.h"

namespace NXTN {
	Renderable::Renderable() : meshRef(nullptr) {}

	Renderable::Renderable(Mesh* m, Handle<Material> s) : meshRef(m), materialHandle(s) {}

	Renderable::Renderable(std::shared_ptr<Mesh> mesh, Handle<Material> material)
		: materialHandle(material), meshRef(std::move(mesh)) {}
}
