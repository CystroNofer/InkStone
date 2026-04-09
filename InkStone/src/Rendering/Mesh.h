#pragma once

#include "VertexArray.h"
#include "RenderingDataBuffer.h"

namespace NXTN {
	class Mesh
	{
	public:
		Mesh(VertexArray* va, IndexBuffer* ib);
		~Mesh() {}

		void Bind() const;
		void Unbind() const;

		const VertexArray& GetVertexArray() const;
		const IndexBuffer& GetIndexBuffer() const;

	private:
		const std::unique_ptr<VertexArray> m_VertexArray;
		const std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};
}
