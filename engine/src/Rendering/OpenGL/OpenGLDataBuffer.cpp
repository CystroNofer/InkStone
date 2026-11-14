#include "pch.h"

#include "OpenGLDataBuffer.h"
#include <stdexcept>

namespace NXTN {
	// Vertex buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, const unsigned int count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_ObjectID);
		if (!m_ObjectID) { throw std::runtime_error("OpenGL vertex-buffer creation failed."); }
		glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
		GLint64 allocatedBytes = 0;
		glGetNamedBufferParameteri64v(m_ObjectID, GL_BUFFER_SIZE, &allocatedBytes);
		if (allocatedBytes != static_cast<GLint64>(count) * sizeof(float)) {
			glDeleteBuffers(1, &m_ObjectID);
			throw std::runtime_error("OpenGL vertex-buffer allocation failed.");
		}
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
		if (!m_ObjectID) { throw std::runtime_error("OpenGL index-buffer creation failed."); }
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ObjectID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		GLint64 allocatedBytes = 0;
		glGetNamedBufferParameteri64v(m_ObjectID, GL_BUFFER_SIZE, &allocatedBytes);
		if (allocatedBytes != static_cast<GLint64>(count) * sizeof(unsigned int)) {
			glDeleteBuffers(1, &m_ObjectID);
			throw std::runtime_error("OpenGL index-buffer allocation failed.");
		}
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
