#include "pch.h"

#include "Renderable.h"

namespace NXTN {
	Renderable::Renderable() : meshRef(nullptr) {}

	Renderable::Renderable(Mesh* m, Shader* s) : meshRef(m), shaderRef(s) {}
}