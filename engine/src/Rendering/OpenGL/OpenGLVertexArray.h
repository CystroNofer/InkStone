#pragma once

#include "Rendering/VertexArray.h"
#include "OpenGLDataBuffer.h"

#include <GLAD/glad.h>

namespace NXTN {

	static GLenum ShaderDataTypeToOpenGLType(VertexDataType type)
	{
		switch (type)
		{
		case VertexDataType::Int:   return GL_INT;
		case VertexDataType::Float: return GL_FLOAT;
		default:                    return -1;
		}
	}

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray(OpenGLVertexBuffer* vertexBuffer, const VertexArrayLayout& layout);
		OpenGLVertexArray(OpenGLVertexBuffer* vertexBuffer, const std::initializer_list<VertexAtrribute>& il);
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		unsigned int m_ObjectID;
		std::shared_ptr<OpenGLVertexBuffer> m_VertexBuffer;
		VertexArrayLayout m_Layout;
	};
}

