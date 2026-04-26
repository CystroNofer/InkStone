#pragma once

#include "Core.h"
#include "RenderingDataBuffer.h"

namespace NXTN {

	enum class VertexDataType
	{
		None = 0, Int, Float
	};

	static unsigned int VertexDataTypeSize(VertexDataType type)
	{
		switch (type)
		{
		case VertexDataType::Int:   return sizeof(int);
		case VertexDataType::Float: return sizeof(float);
		default:                    return -1;
		}
	}

	struct VertexAtrribute
	{
		VertexDataType type;  // Base type of the attribute
		unsigned int count;   // Number of values (Combined with *type* to form compound values)
		std::string name;
	};

	class VertexArrayLayout
	{
	public:
		VertexArrayLayout();
		VertexArrayLayout(const std::initializer_list<VertexAtrribute>& il);
		~VertexArrayLayout();

		void AddAttribute(VertexDataType type, unsigned int count, const std::string& name);
		inline const std::vector<VertexAtrribute>& GetAttributes() const { return m_Layout; }
		inline unsigned int GetStride() const { return m_Stride; }

	private:
		std::vector<VertexAtrribute> m_Layout;
		unsigned int m_Stride;  // The sum of the sizes of all attributes (The size of each vertex)
	};

	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static VertexArray* Create(VertexBuffer* vertexBuffer, const VertexArrayLayout& layout);
		static VertexArray* Create(VertexBuffer* vertexBuffer, const std::initializer_list<VertexAtrribute>& il);
	};
}
