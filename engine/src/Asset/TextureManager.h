#pragma once

#include "Container/HandleMap.h"
#include "Rendering/Texture.h"

namespace NXTN {
	class TextureManager
	{
	public:
		~TextureManager() {}

		static Handle<Texture2D> Create(const std::string& filepath);
		static Texture2D* Get(Handle<Texture2D> th);
		static bool Validate(Handle<Texture2D> th);

	private:
		TextureManager() = default;

		static HandleMap<Texture2D> s_Map2D;
		//static HandleMap<Texture3D> s_Map3D;
	};
}
