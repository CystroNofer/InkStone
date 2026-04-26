#include "pch.h"

#include "APISetting.h"

#include "OpenGL/OpenGLFrameBuffer.h"

namespace NXTN {
	FrameBuffer* FrameBuffer::Create(unsigned int width, unsigned int height)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return (FrameBuffer*) new OpenGLFrameBuffer(width, height);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}
}
