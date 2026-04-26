#pragma once

#include "Log.h"

namespace NXTN {
	class Texture
	{
	public:
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Texture2D* Create(const std::string& filepath);
	};
}
