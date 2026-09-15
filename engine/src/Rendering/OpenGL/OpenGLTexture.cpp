#include "pch.h"

#include "OpenGLTexture.h"

#include <stb_image.h>

#include <GLAD/glad.h>

namespace NXTN {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath)
		: m_FilePath(filepath)
	{
		/*
		While stbi_load loads files from top to bottom
		OpenGL expect the pixel at the bottom to be loaded first
		*/
		stbi_set_flip_vertically_on_load(1);

		int nChannel = 0;
		unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &nChannel, 0);
		if (!data)
		{
			const char* reason = stbi_failure_reason();
			Log::Warning(
				"Cannot load image at %s: %s",
				filepath.c_str(), reason ? reason : "unknown stb_image error"
			);
			return;
		}

		unsigned int internalFormat = 0;
		unsigned int dataFormat = 0;
		switch (nChannel)
		{
		case 1:
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			break;
		case 3:
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		default:
			Log::Warning("Unsupported image format with %d channels at %s", nChannel, filepath.c_str());
			stbi_image_free(data);
			m_Width = 0;
			m_Height = 0;
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		if (m_RendererID == 0)
		{
			Log::Warning("Failed to create OpenGL texture for %s", filepath.c_str());
			stbi_image_free(data);
			m_Width = 0;
			m_Height = 0;
			return;
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int previousUnpackAlignment = 0;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
		glTextureSubImage2D(
			m_RendererID, 0, 0, 0, m_Width, m_Height,
			dataFormat, GL_UNSIGNED_BYTE, data
		);
		glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		if (m_RendererID != 0)
		{
			glDeleteTextures(1, &m_RendererID);
		}
	}

	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
