#pragma once

#include "Rendering/RenderingDataBuffer.h"

#include <GLAD/glad.h>

namespace NXTN {
	// Vertex buffer
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const float* vertices, const unsigned int count);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual inline unsigned int GetCount() const override { return m_Count; }
		
	private:
		unsigned int m_ObjectID;
		unsigned int m_Count;
	};

	// Index buffer
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const unsigned int* indices, const unsigned int count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual inline unsigned int GetCount() const override { return m_Count; }

	private:
		unsigned int m_ObjectID;
		unsigned int m_Count;
	};
}