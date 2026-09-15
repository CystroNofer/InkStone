#include "pch.h"

#include "FrameBuffer.h"
#include "GraphicsDevice.h"

namespace NXTN {
	FrameBuffer* FrameBuffer::Create(unsigned int width, unsigned int height)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			Log::Warning("Graphics device not initialized");
			return nullptr;
		}
		return device->CreateFrameBuffer(width, height);
	}
}
