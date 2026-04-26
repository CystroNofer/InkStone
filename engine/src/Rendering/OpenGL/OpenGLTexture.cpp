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

		int nChannel;
		unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &nChannel, 0);
		if (!data)
		{
			Log::Error("Cannot load image at %s", filepath.c_str());
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// Linear filter
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// Clamping
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		switch (nChannel)
		{
		case 1:  // Greyscale
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;
		case 3:  // RGB
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:  // RGBA
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			Log::Error("Unknown or unsupported image format with %d channels", nChannel);
			break;
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glBindTextureUnit(slot, m_RendererID);
	}
}
