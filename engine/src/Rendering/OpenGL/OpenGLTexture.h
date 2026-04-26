#pragma once

#include "Rendering/Texture.h"

namespace NXTN {
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& filepath);
		~OpenGLTexture2D();

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual void Bind(unsigned int slot = 0) const override;

	private:
		std::string m_FilePath;
		int m_Width, m_Height;

		unsigned int m_RendererID;
	};
}

