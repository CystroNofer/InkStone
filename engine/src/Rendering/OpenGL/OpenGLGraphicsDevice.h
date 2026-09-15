#pragma once

#include "Rendering/GraphicsDevice.h"

namespace NXTN {
	class OpenGLGraphicsDevice final : public GraphicsDevice
	{
	public:
		Renderer* CreateRenderer() override;
		UI* CreateUI(unsigned int width, unsigned int height) override;

		VertexBuffer* CreateVertexBuffer(const float* vertices, unsigned int count) override;
		IndexBuffer* CreateIndexBuffer(const unsigned int* indices, unsigned int count) override;
		VertexArray* CreateVertexArray(VertexBuffer* vertexBuffer, const VertexArrayLayout& layout) override;
		Texture2D* CreateTexture2D(const std::string& filepath) override;
		FrameBuffer* CreateFrameBuffer(unsigned int width, unsigned int height) override;
		Shader* CreateShader(const ShaderProgramDescriptor& description) override;
	};
}
