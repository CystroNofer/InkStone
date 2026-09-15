#include "pch.h"

#include "RenderingDataBuffer.h"
#include "GraphicsDevice.h"

namespace NXTN {
	VertexBuffer* VertexBuffer::Create(const float* vertices, unsigned int count)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return nullptr;
		}
		return device->CreateVertexBuffer(vertices, count);
	}

	IndexBuffer* IndexBuffer::Create(const unsigned int* indices, unsigned int count)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return nullptr;
		}
		return device->CreateIndexBuffer(indices, count);
	}
}
