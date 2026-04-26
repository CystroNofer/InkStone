#pragma once

#include "Log.h"

namespace NXTN {
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() {}

		virtual void Resize(unsigned int width, unsigned int height) = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual unsigned int GetColorAttachment() const = 0;  // Temp

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		static FrameBuffer* Create(unsigned int width, unsigned int height);
	};
}

