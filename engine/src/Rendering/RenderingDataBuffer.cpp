#include "pch.h"

#include "OpenGL/OpenGLDataBuffer.h"

namespace NXTN {
	VertexBuffer* VertexBuffer::Create(const float* vertices, unsigned int count)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return (VertexBuffer*) new OpenGLVertexBuffer(vertices, count);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(const unsigned int* indices, unsigned int count)
	{
		switch (APISetting::GetGraphicsAPI())
		{
		case GraphicsAPI::None:
			Log::Error("No rendering API specified");
			break;
		case GraphicsAPI::OpenGL:
			return (IndexBuffer*) new OpenGLIndexBuffer(indices, count);
			break;
		default:
			Log::Error("Unsupported rendering API");
			break;
		}

		return nullptr;
	}
}