#pragma once

#include "Core.h"

#include "Log.h"
#include "APISetting.h"

namespace NXTN {
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual unsigned int GetCount() const = 0;

		static VertexBuffer* Create(const float* vertices, unsigned int count);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual unsigned int GetCount() const = 0;

		static IndexBuffer* Create(const unsigned int* vertices, unsigned int count);
	};
}