#include "pch.h"

#include "OpenGLFrameBuffer.h"

#include <GLAD/glad.h>

namespace NXTN {
	OpenGLFrameBuffer::OpenGLFrameBuffer(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height)
	{
		int previousDrawFrameBuffer = 0;
		int previousReadFrameBuffer = 0;
		int previousTexture = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawFrameBuffer);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadFrameBuffer);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// Depth attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthStencilAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment, 0);

		// Check framebuffer status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Warning("Frame buffer incomplete");
		}

		glBindTexture(GL_TEXTURE_2D, previousTexture);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previousDrawFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, previousReadFrameBuffer);
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthStencilAttachment);
	}

	void OpenGLFrameBuffer::Resize(unsigned int width, unsigned int height)
	{
		int previousDrawFrameBuffer = 0;
		int previousReadFrameBuffer = 0;
		int previousTexture = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawFrameBuffer);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadFrameBuffer);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

		m_Width = width;
		m_Height = height;

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color attachment
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// Depth attachment
		glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment, 0);

		// Check framebuffer status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Warning("Frame buffer incomplete");
		}

		glBindTexture(GL_TEXTURE_2D, previousTexture);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previousDrawFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, previousReadFrameBuffer);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		if (!m_IsBound)
		{
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_PreviousDrawFrameBuffer);
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_PreviousReadFrameBuffer);
			glGetIntegerv(GL_VIEWPORT, m_PreviousViewport);
			m_IsBound = true;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Width, m_Height);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		if (!m_IsBound)
		{
			return;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PreviousDrawFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_PreviousReadFrameBuffer);
		glViewport(
			m_PreviousViewport[0], m_PreviousViewport[1],
			m_PreviousViewport[2], m_PreviousViewport[3]
		);
		m_IsBound = false;
	}
}
