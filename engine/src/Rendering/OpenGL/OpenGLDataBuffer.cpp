#include "pch.h"

#include "OpenGLDataBuffer.h"

namespace NXTN {
	// Vertex buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, const unsigned int count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_ObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_ObjectID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Index buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(const unsigned int* indices, const unsigned int count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_ObjectID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ObjectID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_ObjectID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ObjectID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}