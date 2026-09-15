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
		inline bool IsValid() const { return m_RendererID != 0; }

	private:
		std::string m_FilePath;
		int m_Width = 0;
		int m_Height = 0;

		unsigned int m_RendererID = 0;
	};
}

