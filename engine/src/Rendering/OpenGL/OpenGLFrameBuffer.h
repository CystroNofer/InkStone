#pragma once

#include "Rendering/FrameBuffer.h"

namespace NXTN {
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(unsigned int width, unsigned int height);
		~OpenGLFrameBuffer();

		virtual void Resize(unsigned int width, unsigned int height) override;

		virtual inline unsigned int GetWidth() const override { return m_Width; }
		virtual inline unsigned int GetHeight() const override { return m_Height; }

		virtual inline unsigned int GetColorAttachment() const override { return m_ColorAttachment; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		unsigned int m_RendererID;
		unsigned int m_Width, m_Height;
		unsigned int m_ColorAttachment;
		unsigned int m_DepthStencilAttachment;

		mutable int m_PreviousDrawFrameBuffer = 0;
		mutable int m_PreviousReadFrameBuffer = 0;
		mutable int m_PreviousViewport[4] = { 0, 0, 0, 0 };
		mutable bool m_IsBound = false;
	};
}
