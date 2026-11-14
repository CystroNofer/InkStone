#include "pch.h"

#include "OpenGLVertexArray.h"
#include <stdexcept>

namespace NXTN {

	OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexBuffer* vertexBuffer, const VertexArrayLayout& layout)
		: OpenGLVertexArray(std::shared_ptr<OpenGLVertexBuffer>(vertexBuffer), layout) {}

	OpenGLVertexArray::OpenGLVertexArray(std::shared_ptr<OpenGLVertexBuffer> vertexBuffer, const VertexArrayLayout& layout)
		: m_VertexBuffer(std::move(vertexBuffer)), m_Layout(layout)
	{
		glCreateVertexArrays(1, &m_ObjectID);
		if (!m_ObjectID) { throw std::runtime_error("OpenGL vertex-array creation failed."); }
		glBindVertexArray(m_ObjectID);

		m_VertexBuffer->Bind();

		unsigned int index = 0;
		unsigned int stride = m_Layout.GetStride();
		unsigned int offset = 0;
		for (const VertexAtrribute& attribute : m_Layout.GetAttributes())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				attribute.count,
				ShaderDataTypeToOpenGLType(attribute.type),
				GL_FALSE,
				stride,
				(const void*)offset
			);
			offset += VertexDataTypeSize(attribute.type) * attribute.count;

			index++;
		}

		glBindVertexArray(0);
	}

	OpenGLVertexArray::OpenGLVertexArray(OpenGLVertexBuffer* vertexBuffer, const std::initializer_list<VertexAtrribute>& il)
		: OpenGLVertexArray(vertexBuffer, VertexArrayLayout(il)) {}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ObjectID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_ObjectID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}
