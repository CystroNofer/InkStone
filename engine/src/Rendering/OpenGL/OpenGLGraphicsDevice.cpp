#include "pch.h"

#include "OpenGLGraphicsDevice.h"

#include "OpenGLDataBuffer.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderer.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLVertexArray.h"
#include "UI/OpenGL/OpenGLUI.h"

namespace NXTN {
	Renderer* OpenGLGraphicsDevice::CreateRenderer()
	{
		return new OpenGLRenderer();
	}

	UI* OpenGLGraphicsDevice::CreateUI(unsigned int width, unsigned int height)
	{
		return new OpenGLUI(width, height);
	}

	VertexBuffer* OpenGLGraphicsDevice::CreateVertexBuffer(const float* vertices, unsigned int count)
	{
		return new OpenGLVertexBuffer(vertices, count);
	}

	IndexBuffer* OpenGLGraphicsDevice::CreateIndexBuffer(const unsigned int* indices, unsigned int count)
	{
		return new OpenGLIndexBuffer(indices, count);
	}

	VertexArray* OpenGLGraphicsDevice::CreateVertexArray(VertexBuffer* vertexBuffer, const VertexArrayLayout& layout)
	{
		// static_cast is unable to ensure safe typecast of pointers
		// TODO: Add logging
		OpenGLVertexBuffer* openGLVertexBuffer = dynamic_cast<OpenGLVertexBuffer*>(vertexBuffer);

		if (openGLVertexBuffer == nullptr)
		{
			return nullptr;
		}

		return new OpenGLVertexArray(openGLVertexBuffer, layout);
	}

	Texture2D* OpenGLGraphicsDevice::CreateTexture2D(const std::string& filepath)
	{
		OpenGLTexture2D* texture = new OpenGLTexture2D(filepath);
		if (!texture->IsValid())
		{
			delete texture;
			return nullptr;
		}
		return texture;
	}

	FrameBuffer* OpenGLGraphicsDevice::CreateFrameBuffer(unsigned int width, unsigned int height)
	{
		return new OpenGLFrameBuffer(width, height);
	}

	Shader* OpenGLGraphicsDevice::CreateShader(const ShaderProgramDescriptor& description)
	{
		OpenGLShader* shader = new OpenGLShader(description);
		if (!shader->IsValid())
		{
			delete shader;
			return nullptr;
		}
		return shader;
	}
}
