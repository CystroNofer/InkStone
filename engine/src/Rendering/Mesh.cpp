#include "pch.h"

#include "Mesh.h"

namespace NXTN {
	Mesh::Mesh(VertexArray* va, IndexBuffer* ib)
		: m_VertexArray(va), m_IndexBuffer(ib)
	{

	}

	void Mesh::Bind() const
	{
		m_VertexArray->Bind();
		m_IndexBuffer->Bind();
	}

	void Mesh::Unbind() const
	{
		m_VertexArray->Unbind();
		m_IndexBuffer->Unbind();
	}

	const VertexArray& Mesh::GetVertexArray() const
	{
		return *m_VertexArray;
	}

	const IndexBuffer& Mesh::GetIndexBuffer() const
	{
		return *m_IndexBuffer;
	}
}
