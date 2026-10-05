#include "pch.h"

#include "Renderable.h"

namespace NXTN {
	Renderable::Renderable() : meshRef(nullptr) {}

	Renderable::Renderable(Mesh* m, Handle<Material> s) : meshRef(m), materialHandle(s) {}
}