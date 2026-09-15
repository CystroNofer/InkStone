#include "pch.h"

#include "VertexArray.h"
#include "GraphicsDevice.h"

namespace NXTN {
	// Vertex Array Layout
	VertexArrayLayout::VertexArrayLayout() : m_Layout(0), m_Stride(0) {}

	VertexArrayLayout::VertexArrayLayout(const std::initializer_list<VertexAtrribute>& il)
	{
		m_Stride = 0;

		for (const VertexAtrribute& attribute : il)
		{
			m_Stride += VertexDataTypeSize(attribute.type) * attribute.count;
		}

		m_Layout = il;
	}

	VertexArrayLayout::~VertexArrayLayout() {}

	void VertexArrayLayout::AddAttribute(VertexDataType type, unsigned int count, const std::string& name)
	{
		m_Layout.push_back({ type, count, name });

		m_Stride += VertexDataTypeSize(type) * count;
	}

	// Vertex Array
	VertexArray* VertexArray::Create(VertexBuffer* vertexBuffer, const VertexArrayLayout& layout)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return nullptr;
		}
		return device->CreateVertexArray(vertexBuffer, layout);
	}

	// Vertex Array
	VertexArray* VertexArray::Create(VertexBuffer* vertexBuffer, const std::initializer_list<VertexAtrribute>& il)
	{
		return Create(vertexBuffer, VertexArrayLayout(il));
	}
}
